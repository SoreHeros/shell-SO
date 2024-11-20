//
// Created by heros on 13/11/24.
//

#include "communicate.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <sys/shm.h>

#define BUFFER_SIZE 20

typedef struct{
    mtx_t semap;
    mtx_t list;
    int elements;
    int connected;
    char buffer[BUFFER_SIZE];
}mem;

void communicate(char ** tokens, int){
    int id = shmget(atoi(tokens[0]), sizeof(mem), 0777 | IPC_CREAT | IPC_EXCL);
    mem * p;
    if(id == -1){
        id = shmget(atoi(tokens[0]), sizeof(mem), 0777 | IPC_CREAT);
        p = shmat(id, NULL, 0);
    }else{//init
        p = shmat(id, NULL, 0);
        mtx_init(&p->semap, mtx_plain);
        mtx_init(&p->semap, mtx_plain);
    }

}

void communicate_help(){

}

