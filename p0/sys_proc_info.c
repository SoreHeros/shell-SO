//
// Created by heros on 19/09/24.
//

#include "sys_proc_info.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <string.h>
#include "../lists.h"

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

    struct utsname utsname;

    uname(&utsname);



    if(flags.s)
        printf("%s\n", utsname.sysname);
    if(flags.n)
        printf("%s\n", utsname.nodename);
    if(flags.r)
        printf("%s\n", utsname.release);
    if(flags.v)
        printf("%s\n", utsname.version);
    if(flags.m)
        printf("%s\n", utsname.machine);
    if(flags.p)
        printf("a\n");
    if(flags.i)
        printf("a\n");
    if(flags.o)
        printf("a\n");

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
    printf("-m:\tprints the machine name\n");
    printf("-p:\tprints the processor type\n");
    printf("-i:\tprints the hardware platform\n");
    printf("-o:\tprints the operative system\n");
}