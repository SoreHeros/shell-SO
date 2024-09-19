//
// Created by heros on 19/09/24.
//

#include "echo.h"
#include <stdio.h>

void echo(char ** tokens, int token_len){
    for(int i = 0; i < token_len; i++)
        printf("%s\n", tokens[i]);
}

void echo_help(){

}