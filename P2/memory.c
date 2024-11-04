//
// Created by heros on 30/10/24.
//

#include "memory.h"

#include <stdint.h>

#include "../P0/lists.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum{
    MALLOC, SHARED, MAPPED
}alloc_type;

typedef struct alloc{
    void * addr;
    time_t time;
    unsigned int size;
    alloc_type type;
} * alloc;

list blocks_list;

void mallocs_init(){
    blocks_list = list_init();
}
void mallocs_exit(){
    for(int i = 0; i < list_length(blocks_list); i++){
        alloc a = list_get(blocks_list, i);
        free(a->addr);
        free(a);
    }
    list_free(blocks_list);
}

long long int interpretNumberFormat(char * s){
    int len = strlen(s);
    if(len < 3)
        return strtol(s, NULL, 10);

    if(s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
        return strtol(&s[2], NULL, 16);
    if(s[0] == '0' && (s[1] == 'o' || s[1] == 'O'))
        return strtol(&s[2], NULL, 8);
    if(s[0] == '0' && (s[1] == 'b' || s[1] == 'B'))
        return strtol(&s[2], NULL, 2);

    return strtol(s, NULL, 10);
}

void printMemBlocks(){
    int len = list_length(blocks_list);

    if(len == 0){
        printf("Block list is empty\n");
        return;
    }

    for(int i = 0; i < len; i++){
        alloc a = list_get(blocks_list, i);
        struct tm * time = localtime(&a->time);
        printf("%04i/%02i/%02i-%02i:%02i:%02i\t", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);
        printf("0x%016lx 0x%08x(%10u) \n", (uintptr_t) a->addr, a->size, a->size);
    }
}

void allocateMalloc(unsigned size){
    alloc a = malloc(sizeof(struct alloc));
    if(a == NULL){
        perror("ERROR ON ALLOCATE (new list element)");
        return;
    }
    time(&a->time);
    a->addr = malloc(size);
    if(a->addr == NULL){
        perror("ERROR ON ALLOCATE (allocating amemory)");
        free(a);
        return;
    }
    //for(int i = 0; i < size; i++)
        //((uint8_t *)a->addr)[i] = i;
    a->size = size;
    a->type = MALLOC;
    list_append(blocks_list, a);
    printf("allocated %i bytes on %p\n", size, a->addr);
}

void allocate(char ** tokens, int token_number){//todo allocate -createshared -shared -mmap

    if(token_number == 0){
        printMemBlocks();
        return;
    }

    for(int i = 0; i < token_number; i++){
        long long int res = interpretNumberFormat(tokens[i]);
        allocateMalloc(res);
    }
}
void allocate_help(){
    printf("\tallocate [-malloc n] [-mmap file perm] [-createshared cl n] [-shared  cl]\n");
    printf("empty:\tshows the malloced memory\n");
    printf("n:\tnumber (can be decimal, hex(0x), bin(0b), oct(0o))\n");
    printf("file:\tfile name\n");
    printf("perm:\tfile permissions\n");
    printf("cl:\tshared memory key\n");
    printf("-malloc:\tasigns a memory block\n");
    printf("-mmap:\tdumps the file contents into a new memory block\n");
    printf("-createshared:\tasigns a shared memory block with the specified key\n");
    printf("-shared:\tattaches the memory block with the specified key\n");
}

void memdump(char ** tokens, int token_number){

    if(token_number == 0){
        printMemBlocks();
        return;
    }

    if(token_number < 2 || 3 < token_number){
        fprintf(stderr, "ERROR ON PARSING: ILLEGAL NUMBER OF ARGUMENTS\n");
        return;
    }

    uint8_t * addr = (uint8_t *)interpretNumberFormat(tokens[token_number - 2]);
    long long int size =  interpretNumberFormat(tokens[token_number - 1]);
    char safe = 1;
    if(!strcmp(tokens[0], "-unsafe"))
        safe = 0;

    //test if is malloced
    char ismalloced = 0;
    for(int i = 0; i < list_length(blocks_list); i++){
        alloc a = list_get(blocks_list, i);
        if(a->addr <= (void *)addr && (void *)addr + size <= a->addr + a->size){
            if(size <= 0)
                size = a->addr + a->size - (void *)addr;
            ismalloced = 1;
            break;
        }
    }
    if(!ismalloced && safe){
        fprintf(stderr, "ERROR ON SAFE MODE: ADDR ISN'T ALLOCATED OR SIZE IS TOO BIG (use -unsafe to ignore)\n");
        return;
    }


    //print starting line
  //printf(      "0x0000000000000000     f0 f1 f2 f3 f4 f5 f6 f7   f8 f9 fa fb fc fd fe ff     [01234567 89abcdef]\n");
    //printf("                        0  1  2  3  4  5  6  7    8  9  a  b  c  d  e  f\n");

    for(uintptr_t p = (uintptr_t)addr & ~(uintptr_t)0xf; p < (uintptr_t)(addr + size); p += 0x10){
        printf("0x%016lx     ", p);
        uint8_t bytes[0x10] = {0};


        //get bytes
        for(int i = 0; i < 0x10; i++)
            if(!(p + i < (uintptr_t)addr || (uintptr_t)(addr + size) <= p + i))
                bytes[i] = ((uint8_t *)p)[i];

        //print bytes
        for(int i = 0; i < 0x8; i++)
            if(p + i < (uintptr_t)addr || (uintptr_t)(addr + size) <= p + i)
                printf("   ");
            else
                printf("%02x ", bytes[i]);

        printf("  ");

        for(int i = 0x8; i < 0x10; i++)
            if(p + i < (uintptr_t)addr || (uintptr_t)(addr + size) <= p + i)
                printf("   ");
            else
                printf("%02x ", bytes[i]);

        printf("    [");
        for(int i = 0; i < 0x8; i++)
            if(p + i < (uintptr_t)addr || (uintptr_t)(addr + size) <= p + i)
                printf(" ");
            else if(bytes[i] < 32 || bytes[i] == 127 || bytes[i] == 255)
                printf(".");
            else
                printf("%c", bytes[i]);

        printf(" ");

        for(int i = 0x8; i < 0x10; i++)
            if(p + i < (uintptr_t)addr || (uintptr_t)(addr + size) <= p + i)
                printf(" ");
            else if(bytes[i] < 32 || bytes[i] == 127 || bytes[i] == 255)
                printf(".");
            else
                printf("%c", bytes[i]);
        printf("]\n");
    }
}
void memdump_help(){
    printf("\tmemdump [-unsafe] addr size\n");
    printf("empty:\tshows the malloced memory\n");
    printf("addr:\tthe address of the memory to be dumped (can be decimal, hex(0x), bin(0b), oct(0o))\n");
    printf("size:\tthe number of bytes to be dumped (can be decimal, hex(0x), bin(0b), oct(0o)) (0 for everything untill end of block)\n");
    printf("-unsafe:\tignores the segfault precautions\n");
}