//
// Created by heros on 19/09/24.
//

#include "basic_functions.h"
#include <stdio.h>

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
