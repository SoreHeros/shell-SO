//
// Created by heros on 19/09/24.
//

#include "sys_proc_info.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

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

//       uname [OPTION]...
//
//DESCRIPTION
//       Print certain system information.  With no OPTION, same as -s.
//
//       -a, --all
//              print  all  information,  in the following order, except omit -p
//              and -i if unknown:
//
//       -s, --kernel-name
//              print the kernel name
//
//       -n, --nodename
//              print the network node hostname
//
//       -r, --kernel-release
//              print the kernel release
//
//       -v, --kernel-version
//              print the kernel version
//
//       -m, --machine
//              print the machine hardware name
//
//       -p, --processor
//              print the processor type (non-portable)
//
//       -i, --hardware-platform
//              print the hardware platform (non-portable)
//
//       -o, --operating-system
//              print the operating system
void infosys(char ** tokens, int token_number){
    struct{
        char s;
        char n;
        char r;
        char v;
        char m;
        char p;
        char i;
        char o;
    }flags = {0};

    for(int i = 0; i < token_number; i++){
        //test all
        if(!strcmp(tokens[i], "-a") || !strcmp(tokens[i], "--all")){
            flags.s = 1;
            flags.n = 1;
            flags.r = 1;
            flags.v = 1;
            flags.m = 1;
            flags.p = 1;
            flags.i = 1;
            flags.o = 1;
            break;
        }

        //test case by case
        if(!strcmp(tokens[i], "-s") || !strcmp(tokens[i], "--kernel-name"))
            flags.s = 1;
        else if(!strcmp(tokens[i], "-n") || !strcmp(tokens[i], "--nodename"))
            flags.n = 1;
        else if(!strcmp(tokens[i], "-r") || !strcmp(tokens[i], "--kernel-release"))
            flags.r = 1;
        else if(!strcmp(tokens[i], "-v") || !strcmp(tokens[i], "--kernel-version"))
            flags.v = 1;
        else if(!strcmp(tokens[i], "-m") || !strcmp(tokens[i], "--machine"))
            flags.m = 1;
        else if(!strcmp(tokens[i], "-p") || !strcmp(tokens[i], "--processor"))
            flags.p = 1;
        else if(!strcmp(tokens[i], "-i") || !strcmp(tokens[i], "--hardware-platform"))
            flags.i = 1;
        else if(!strcmp(tokens[i], "-o") || !strcmp(tokens[i], "--operating-system"))
            flags.o = 1;
    }

    //comprobar caso por defecto
    if(!flags.s && !flags.i && !flags.m && !flags.n && !flags.o && !flags.p && !flags.r && !flags.v)
        flags.s = 1;

    //todo info del sistema
}
void infosys_help(){
    printf("\tinfosys [-a|-s|-n|-r|-v|-m|-p|-i|-o]\n");
    printf("empty:\tsame as -s\n");
    printf("-a:\tprints all system info\n");
    printf("-s:\tprints kernel name\n");
    printf("-n:\tprints the network node hostname\n");
    printf("-r:\tprints the kernel release\n");
    printf("-v:\tprints the kernel version\n");
    printf("-m:\t");
}