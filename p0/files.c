//
// Created by heros on 19/09/24.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lists.h"

list open_files;
typedef enum {READ = 0x1, WRITE = 0x2, BINARY = 0x4, }MODE;

typedef struct{
    char * name;
    FILE * stream;
    MODE mode;
}file;

#include "files.h"

void files_init(){
    open_files = list_new();

    file *f1 = malloc(sizeof(file));

    f1->name = strdup("stdin");
    f1->stream = stdin;

    list_append(&open_files, f1);

    file *f2 = malloc(sizeof(file));

    f2->name = strdup("stdout");
    f2->stream = stdout;

    list_append(&open_files, f2);

    file *f3 = malloc(sizeof(file));

    f3->name = strdup("stderr");
    f3->stream = stderr;

    list_append(&open_files, f3);
}
void files_exit(){
    for(int i = 0; i < list_length(open_files); i++){
        file * f = list_get(open_files, i);
        free(f->name);
        fclose(f->stream);
    }

    list_free(&open_files);
}

void open(char ** tokens, int token_number){
    FILE * f = fopen(tokens[0], "r");

    if(f == NULL){
        perror("ERROR AL ABRIR EL ARCHIVOS:");
        return;
    }

    file * opened_file = malloc(sizeof(file));

    opened_file->stream = f;
    opened_file->name = strdup(tokens[0]);

    list_append(&open_files, opened_file);
}
void open_help(){
    printf("test");
}
void close(char ** tokens, int token_number){
    for(int i = 0; i < list_length(open_files); i++){
        file * f = list_get(open_files, i);
        if(!strcmp(tokens[0], f->name)){
            free(f->name);
            fclose(f->stream);
            free(f);
            list_remove(&open_files, i);
            i--;
        }
    }
}
void close_help(){
    printf("test");
}
void list_files(char ** tokens, int token_number){
    for(int i = 0; i < list_length(open_files); i++){
        file * f = list_get(open_files, i);
        printf("%s\n", f->name);
    }
}
void list_files_help(){
    printf("test");
}