//
// Created by heros on 17/10/24.
//

#include "dirs.h"
#include <stdio.h>

void makefile(char **, int){

}
void makefile_help(){
    printf("\tmakefile [name]\n");
    printf("name:\tcreates a file with that name\n");
}


void makedir(char **, int){

}
void makedir_help(){
    printf("\tmakedir [name]\n");
    printf("name:\tcreates a directory with that name\n");
}


void listfile(char **, int){

}
void listfile_help(){
    printf("\tlistfile [-long|-acc|-link] name [...]\n");
    printf("empty:\tname and size\n");
    printf("name:\tthe name of the file to analyze\n");
    printf("-long:\tadds creation date, inode, symbolic link number, creator and group name and permissions\n");
    printf("-acc:\tadds access date\n");
    printf("-link:\tadds the symbolic path\n");
}


void cwd(char **, int){

}
void cwd_help(){
    printf("\tcwd\n");
    printf("empty:\tshows the current working directory\n");
}


void listdir(char **, int){

}
void listdir_help(){
    printf("\tlistdir [-long|-acc|-link|-hid] dir [...]\n");
    printf("empty:\tshows the directory's files\n");
    printf("dir:\tdirectory name\n");
    printf("-long:\tadds creation date, inode, symbolic link number, creator and group name and permissions\n");
    printf("-acc:\tadds access time\n");
    printf("-link:\tadds the symbolic path\n");
    printf("-hid:\tadds hidden files\n");
}


void reclist(char **, int){

}
void reclist_help(){//todo añadir descripciones a todos los comandos justo después de como se llaman en un print aparte
    printf("\treclist [-long|-acc|-link|-hid] dir [...]\n");
    printf("empty:\tshows the directory's files\n");
    printf("dir:\tdirectory name\n");
    printf("-long:\tadds creation date, inode, symbolic link number, creator and group name and permissions\n");
    printf("-acc:\tadds access time\n");
    printf("-link:\tadds the symbolic path\n");
    printf("-hid:\tadds hidden files\n");
}


void revlist(char **, int){

}
void revlist_help(){
    printf("\trecvlist [-long|-acc|-link|-hid] dir [...]\n");
    printf("empty:\tshows the directory's files\n");
    printf("dir:\tdirectory name\n");
    printf("-long:\tadds creation date, inode, symbolic link number, creator and group name and permissions\n");
    printf("-acc:\tadds access time\n");
    printf("-link:\tadds the symbolic path\n");
    printf("-hid:\tadds hidden files\n");
}


void erase(char **, int){

}
void erase_help(){
    printf("\terase name [...]\n");
    printf("name:\tname of the file to be errased\n");
}


void delrec(char **, int){

}
void delrec_help(){
    printf("\tdelrec name [...]\n");
    printf("name:\tname of the file to be errased\n");
}