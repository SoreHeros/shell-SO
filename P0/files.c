//
// Created by heros on 19/09/24.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "../utils/lists.h"
#include "../utils/shared_vars.h"
#include "files.h"

void list_files(){
    for(int i = 0; i < list_length(files); i++){
        file * f = list_get(files, i);

        printf("%3i: %30s ", f->fd, f->name);

        int mode = fcntl(f->fd, F_GETFL);

        if(mode & O_RDWR)
            printf("O_RDWR ");
        else if(mode & O_WRONLY)
            printf("O_WRONLY ");
        else
            printf("O_RDONLY ");

        if(mode & O_CREAT)
            printf("O_CREAT ");

        if(mode & O_APPEND)
            printf("O_APPEND ");

        if(mode & O_EXCL)
            printf("O_EXCL ");

        if(mode & O_TRUNC)
            printf("O_TRUNC ");

        printf("\n");
    }
}
void open_command(char ** tokens, int token_number){

    if(token_number == 0){
        list_files();
        return;
    }

    int mode = 0;

    for(int i = 1; i < token_number; i++){
        if(!strcmp(tokens[i], "cr"))
            mode |= O_CREAT;
        else if(!strcmp(tokens[i], "ap"))
            mode |= O_APPEND;
        else if(!strcmp(tokens[i], "ex"))
            mode |= O_EXCL;
        else if(!strcmp(tokens[i], "ro"))
            mode |= O_RDONLY;
        else if(!strcmp(tokens[i], "rw"))
            mode |= O_RDWR;
        else if(!strcmp(tokens[i], "wo"))
            mode |= O_WRONLY;
        else if(!strcmp(tokens[i], "tr"))
            mode |= O_TRUNC;

    }

    int fd = open(tokens[0], mode);
    if(fd < 0){
        perror("ERROR AL ABRIR EL ARCHIVO");
        return;
    }

    file * opened_file = malloc(sizeof(file));

    opened_file->fd = fd;
    opened_file->name = strdup(tokens[0]);

    //list_append(open_files, opened_file);
    list_add(files, opened_file->fd,opened_file);
}
void open_help(){
    printf("\topen [file] [cr|ap|ex|ro|rw|wo|tr] \n");
    printf("empty:\tprints the open files\n");
    printf("file:\tthe file that you want to open\n");
    printf("cr:\tmode crate\n");
    printf("ap:\tmode append\n");
    printf("ex:\tmode excl\n");
    printf("ro:\tmode read only\n");
    printf("rw:\tmode read write\n");
    printf("wo:\tmode write only\n");
    printf("tr:\tmode truncate\n");
}
void close_command(char ** tokens, int token_number){
    if(token_number <= 0)
        return;

    int ds = atoi(tokens[0]);

    if(ds < 3){
        fprintf(stderr,"ERROR AL CERRAR ARCHIVO: ES UNA MALA IDEA\n");
        return;
    }

    for(int i = 3; i < list_length(files); i++){
        file * f = list_get(files, i);
        if(ds == f->fd){
            close(ds);
            free(f->name);
            free(f);
            list_remove(files, i);
            i--;
        }
    }
}
void close_help(){
    printf("\tclose [descriptior]\n");
    printf("closes the chosen file\n");
}
void cd(char ** tokens, int token_number) {
    if(token_number){
        if(chdir(tokens[0]))
            perror("ERROR AL CAMBIAR EL DIRECTIORIO DE TRABAJO");
    }else{
        char * pathname;
        if((pathname =getcwd(NULL, 0)) == NULL){
            perror("ERROR AL OBTENER EL DIRECTORIO DE TRABAJO");
            return;
        }
        printf("%s\n", pathname);
        free(pathname);
    }
}
void cd_help(){
    printf("\tcd [path]\n");
    printf("empty:\tprints the current working directory\n");
    printf("path:\tchanges the working directory to the chosen one\n");
}
void dup_command(char ** tokens, int token_number){
    if(!token_number)
        return;
    int oldfd = atoi(tokens[0]);
    int newfd = dup(oldfd);

    if(newfd < 0){
        perror("ERROR AL DUPLICAR EL ARCHIVO");
        return;
    }

    file  * f = malloc(sizeof(file));
    f->fd = newfd;

    for(int i = 0; i < list_length(files); i++){
        file * aux = list_get(files, i);
        if(aux->fd == oldfd) {
            //copiar el string antiguo con el (dup)
            char dup[] = "(dup)";
            f->name = malloc(sizeof(dup) + strlen(aux->name));
            f->name[0] = '\0';
            strcat(f->name, dup);
            strcat(f->name, aux->name);
            printf("duplicated file %s on file descriptor %i\n", aux->name, f->fd);
            break;
        }
    }
    list_add(files, f->fd, f);
}
void dup_help(){
    printf("\tdup [descriptor]\n");
    printf("duplicates the chosen file\n");
}