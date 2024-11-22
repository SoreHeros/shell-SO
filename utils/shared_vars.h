//
// Created by heros on 20/11/24.
//

#ifndef SHARED_VARS_H
#define SHARED_VARS_H

#include <stddef.h>
#include <sys/types.h>

#include "lists.h"

#define PAGE_NONE            0
#define PAGE_R               1
#define PAGE_W               2
#define PAGE_RW              3
#define PAGE_X               4
#define PAGE_RX              5
#define PAGE_RWX             7
#define PAGE_SHARED          8
#define PAGE_PRIVATE        16
#define PAGE_STACK          32

typedef struct{
    void                    *start;
    size_t                   length;
    unsigned char            perms;
    char                     name[];
}page;

typedef enum {READ = 0x1, WRITE = 0x2, BINARY = 0x4, }MODE;

typedef struct{
    int fd;
    char * name;
}file;

extern list blocks;
extern list files;
extern list history;

extern list get_pmap();
void print_colored_pointer(list pmap, void * addr);
extern char * get_page_perms(unsigned char perms);

void shared_vars_init();
void shared_vars_exit();

#endif //SHARED_VARS_H
