//
// Created by heros on 19/09/24.
//

#include "sys_proc_info.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void pid(char ** tokens, int token_len){
    printf("%i\n", getpid());
}
void pid_help(){
    printf("\tpid\nprints this shell's pid number\n");
}

void ppid(char ** tokens, int token_len){
    printf("%i\n", getppid());
}
void ppid_help(){
    printf("\tppid\nprints this shell's partent pid number\n");
}