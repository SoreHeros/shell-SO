//
// Created by heros on 30/10/24.
//

#include "mallocs.h"

#include "../P0/lists.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum{
    MALLOC, SHARED, MAPPED
}alloc_type;

typedef struct alloc{
    void * addr;
    time_t time;
    int size;
    alloc_type type;
} * alloc;

list malloc_list;

void mallocs_init(){
    malloc_list = list_init();
}
void mallocs_exit(){
    for(int i = 0; i < list_length(malloc_list); i++){
        alloc a = list_get(malloc_list, i);
        free(a->addr);
        free(a);
    }
    list_free(malloc_list);
}

void allocate(char ** tokens, int token_number){

}
void allocate_help(){
    printf("\tallocate [-malloc|-createshared|-shared] name [...]\n");
    printf("name:\tthe name of the file to analyze\n");
    printf("empty:\tname and size\n");
    printf("-long:\tadds creation date, inode, symbolic link number, creator and group name and permissions\n");
    printf("-acc:\tadds access date\n");
    printf("-link:\tadds the symbolic path\n");
}