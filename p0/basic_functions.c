//
// Created by heros on 19/09/24.
//

#include "basic_functions.h"
#include <stdio.h>
#include <string.h>

void echo(char ** tokens, int token_len){
    for(int i = 0; i < token_len; i++)
        printf("%s\n", tokens[i]);
}
void echo_help(){
    printf("\techo [tokens]\nprints each token that it recives on a new line\n");
}

void quit(char ** tokens, int token_number){
    printf("quitting\n");
}
void quit_help(){
    printf("\tquit\nquits the shell\n");
}


void exit_command(char ** tokens, int token_number){
    printf("exitting\n");
}
void exit_help(){
    printf("\texit\nexits the shell\n");
}

void bye(char ** tokens, int token_number){
    printf("goodbye!\n");
}
void bye_help(){
    printf("\tbye\nsays goodbye, then closes the shell\n");
}

void authors(char ** tokens, int token_number){
    struct{
        char n;
        char l;
    }flags = {0};

    for(int i = 0; i < token_number; i++){
        if(!strcmp(tokens[i], "-n"))
            flags.n = 1;
        else if(!strcmp(tokens[i], "-l"))
            flags.l = 1;
    }

    if(!flags.l && !flags.n)
        flags.l = flags.n = 1;

    if(flags.n)
        printf("Heros Vicente Gonzalez\n");
    if(flags.l)
        printf("heros.vicente@udc.es\n");
}
void authors_help(){
    printf("\tauthors [-l|-n]\nempty:\tprints author's name and login\n-n:\tprints author's name\n-l\tprints author's login\n");
}