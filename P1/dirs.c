//
// Created by heros on 17/10/24.
//

#include "dirs.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <linux/limits.h>

#include <dirent.h>


void makefile(char ** tokens, int token_number){
    int fd[token_number];
    int i;
    for(i = 0; i < token_number; i++){
        fd[i] = creat(tokens[i], S_IRUSR | S_IWUSR);
        if(fd[i] < 0){
            fprintf(stderr, "ERROR AL CREAR EL ARCHIVO %s", tokens[i]);
            perror(": ");
            break;
        }
    }
    for(int j = i - 1; j >= 0; j--)
        if(close(fd[j]))
            perror("ERROR EN EL PROCESO DE CREAR ARCHIVO");
}
void makefile_help(){
    printf("\tmakefile name [...]\n");
    printf("name:\tcreates a file with that name\n");
}


void makedir(char ** tokens, int token_number){
    for(int i = 0; i < token_number; i++){
        if(mkdir(tokens[i], 0700)){
            fprintf(stderr, "ERROR AL CREAR EL DIRECTORIO %s", tokens[i]);
            perror(": ");
        }
    }
}
void makedir_help(){
    printf("\tmakedir name [...]\n");
    printf("name:\tcreates a directory with that name\n");
}

void listfilelocal(char lng, char acc, char lnk, char * file){
    struct stat sb;
        if(lstat(file,&sb)){
            fprintf(stderr, "ERROR AL LEER EL ESTADO DE %s", file);
            perror(": ");
            return;;
        }

        if(acc || lng){
            struct tm * time;

            if(acc)
                time = localtime(&sb.st_mtime); //-acc
            else
                time = localtime(&sb.st_atime); //-long

            printf("%04i/%02i/%02i-%02i:%02i\t", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min);

            if(lng){
                printf(" %2ju", sb.st_nlink); //-long

                printf(" (%8ju)\t", sb.st_ino); //-long

                printf(" %10s\t %10s\t", getpwuid(sb.st_uid)->pw_name, getgrgid(sb.st_gid)->gr_name); //-long

                char permisos[] = " --------- ";

                //permisos[0]=LetraTF(m);//implementado como color
                if (sb.st_mode&S_IRUSR) permisos[1]='r';    /*propietario*/
                if (sb.st_mode&S_IWUSR) permisos[2]='w';
                if (sb.st_mode&S_IXUSR) permisos[3]='x';
                if (sb.st_mode&S_IRGRP) permisos[4]='r';    /*grupo*/
                if (sb.st_mode&S_IWGRP) permisos[5]='w';
                if (sb.st_mode&S_IXGRP) permisos[6]='x';
                if (sb.st_mode&S_IROTH) permisos[7]='r';    /*resto*/
                if (sb.st_mode&S_IWOTH) permisos[8]='w';
                if (sb.st_mode&S_IXOTH) permisos[9]='x';
                if (sb.st_mode&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
                if (sb.st_mode&S_ISGID) permisos[6]='s';
                if (sb.st_mode&S_ISVTX) permisos[9]='t';

                printf("%s\t", permisos); //-long, reformateado
            }
        }

        printf(" %10jd", sb.st_size); //normal

        int isLink= 0;

        switch (sb.st_mode & S_IFMT) {
        case S_IFBLK:  printf("\33[35m");               break;//r-b
        case S_IFCHR:  printf("\33[33m");               break;//rg-
        case S_IFDIR:  printf("\33[34m");               break;//--b
        case S_IFIFO:  printf("\33[36m");               break;//-gb
        case S_IFLNK:  printf("\33[32m");  isLink = 1;  break;//-g- si esto imprimir link
        case S_IFREG:  printf("\33[0m" );               break;//rgb
        case S_IFSOCK: printf("\33[37m");               break;//-gb
        default:       printf("\33[31m");               break;//r--
        }

        printf(" %s\33[0m", file);

        if(isLink && lnk){
            printf("\t -> ");
            char path[PATH_MAX];
            realpath(file, path);
            printf("%s", path);
        }

        printf("\n");
}

void listfile(char ** tokens, int token_number){
    struct{
        char lng;
        char acc;
        char lnk;
    }flags = {0};

    char * files[token_number];
    int size = 0;

    for(int i = 0; i < token_number; i++){
        if(!strcmp(tokens[i], "-long"))
            flags.lng = 1;
        else if(!strcmp(tokens[i], "-acc"))
            flags.acc = 1;
        else if(!strcmp(tokens[i], "-link"))
            flags.lnk = 1;
        else{
            files[size] = tokens[i];
            size++;
        }
    }

    char backup[] = ".";
    if(size == 0){
        files[0] = backup;
        size++;
    }

    for(int i = 0; i < size; i++){
        listfilelocal(flags.lng, flags.acc, flags.lnk, files[i]);
    }
}
void listfile_help(){
    printf("\tlistfile [-long|-acc|-link] name [...]\n");
    printf("name:\tthe name of the file to analyze\n");
    printf("empty:\tname and size\n");
    printf("-long:\tadds creation date, inode, symbolic link number, creator and group name and permissions\n");
    printf("-acc:\tadds access date\n");
    printf("-link:\tadds the symbolic path\n");
}


void cwd(char **, int){
    char * pathname;
    if((pathname =getcwd(NULL, 0)) == NULL){
        perror("ERROR AL OBTENER EL DIRECTORIO DE TRABAJO");
        return;
    }
    printf("%s\n", pathname);
    free(pathname);
}
void cwd_help(){
    printf("\tcwd\n");
    printf("prints the current working directory\n");
}
void listdirlocal(char lng, char acc, char lnk, char hid, char * dir){
    DIR *d;
    struct dirent *dirent;
    d = opendir(dir);

    if (d) {
        while ((dirent = readdir(d)) != NULL) {
            if(!hid && dirent->d_name[0] == '.')
                continue;//saltar si es un archivo oculto
            char localpath[PATH_MAX];
            sprintf(localpath, "%s/%s", dir, dirent->d_name);
            listfilelocal(lng, acc, lnk, localpath);
        }
        closedir(d);
    }
}

void listdir(char ** tokens, int token_number){
    struct{
        char lng;
        char acc;
        char lnk;
        char hid;
    }flags = {0};

    char * dirs[token_number];
    int size = 0;

    for(int i = 0; i < token_number; i++){
        if(!strcmp(tokens[i], "-long"))
            flags.lng = 1;
        else if(!strcmp(tokens[i], "-acc"))
            flags.acc = 1;
        else if(!strcmp(tokens[i], "-link"))
            flags.lnk = 1;
        else if(!strcmp(tokens[i], "-hid"))
            flags.hid = 1;
        else{
            dirs[size] = tokens[i];
            size++;
        }
    }

    char backup[] = ".";
    if(size == 0){
        dirs[0] = backup;
        size++;
    }

    for(int i = 0; i < size; i++){
        listdirlocal(flags.lng, flags.acc, flags.lnk, flags.hid, dirs[i]);
    }
}

void listdir_help(){
    printf("\tlistdir [-long|-acc|-link|-hid] dir [...]\n");
    printf("dir:\tdirectory name\n");
    printf("empty:\tshows the directory's files\n");
    printf("-long:\tadds creation date, inode, symbolic link number, creator and group name and permissions\n");
    printf("-acc:\tadds access time\n");
    printf("-link:\tadds the symbolic path\n");
    printf("-hid:\tadds hidden files\n");
}

void reclistLocal(char lng, char acc, char lnk, char hid, char * dir){//todo fix
    DIR *d;
    struct dirent *dirent;

    //list directory
    d = opendir(dir);
    if (d) {
        printf("%s\n", dir);
        while ((dirent = readdir(d)) != NULL) {
            if(!hid && dirent->d_name[0] == '.')
                continue;//saltar si es un archivo oculto
            char * localpath = malloc(strlen(dir) + strlen(dirent->d_name) + 2);//2 strings + \\ + \0
            sprintf(localpath, "%s/%s", dir, dirent->d_name);
            listfilelocal(lng, acc, lnk, localpath);
            free(localpath);
        }
        closedir(d);
    }

    //list subdirectories
    d = opendir(dir);
    if (d) {
        while ((dirent = readdir(d)) != NULL) {
            if((!hid && dirent->d_name[0] == '.') || !strcmp(dirent->d_name, ".") || !strcmp(dirent->d_name, ".."))
                continue;//saltar si es un archivo oculto o si es el propio directorio o el superior
            char * localpath = malloc(strlen(dir) + strlen(dirent->d_name) + 2);//2 strings + \\ + \0
            struct stat sb;
            lstat(localpath,&sb);
            if(sb.st_mode & S_IFLNK){
                free(localpath);
                continue;
            }

            sprintf(localpath, "%s/%s", dir, dirent->d_name);
            reclistLocal(lng, acc, lnk, hid, localpath);
            free(localpath);
        }
        closedir(d);
    }
}

void reclist(char ** tokens, int token_number){
    struct{
        char lng;
        char acc;
        char lnk;
        char hid;
    }flags = {0};


    char * dirs[token_number];
    int size = 0;

    for(int i = 0; i < token_number; i++){
        if(!strcmp(tokens[i], "-long"))
            flags.lng = 1;
        else if(!strcmp(tokens[i], "-acc"))
            flags.acc = 1;
        else if(!strcmp(tokens[i], "-link"))
            flags.lnk = 1;
        else if(!strcmp(tokens[i], "-hid"))
            flags.hid = 1;
        else{
            dirs[size] = tokens[i];
            size++;
        }
    }

    char backup[] = ".";
    if(size == 0){
        dirs[0] = backup;
        size++;
    }

    for(int i = 0; i < size; i++){
        reclistLocal(flags.lng, flags.acc, flags.lnk, flags.hid, dirs[i]);
    }
}
void reclist_help(){//todo añadir descripciones a todos los comandos justo después de como se llaman en un print aparte
    printf("\treclist [-long|-acc|-link|-hid] dir [...]\n");
    printf("dir:\tdirectory name\n");
    printf("empty:\tshows the directory's files\n");
    printf("-long:\tadds creation date, inode, symbolic link number, creator and group name and permissions\n");
    printf("-acc:\tadds access time\n");
    printf("-link:\tadds the symbolic path\n");
    printf("-hid:\tadds hidden files\n");
}


void revlist(char ** tokens, int token_number){

}
void revlist_help(){
    printf("\trecvlist [-long|-acc|-link|-hid] dir [...]\n");
    printf("dir:\tdirectory name\n");
    printf("empty:\tshows the directory's files\n");
    printf("-long:\tadds creation date, inode, symbolic link number, creator and group name and permissions\n");
    printf("-acc:\tadds access time\n");
    printf("-link:\tadds the symbolic path\n");
    printf("-hid:\tadds hidden files\n");
}


void erase(char ** tokens, int token_number){

}
void erase_help(){
    printf("\terase name [...]\n");
    printf("name:\tname of the file to be errased\n");
}


void delrec(char ** tokens, int token_number){

}
void delrec_help(){
    printf("\tdelrec name [...]\n");
    printf("name:\tname of the file to be errased\n");
}