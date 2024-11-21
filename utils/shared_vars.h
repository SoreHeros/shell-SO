//
// Created by heros on 20/11/24.
//

#ifndef SHARED_VARS_H
#define SHARED_VARS_H

#include <stddef.h>
#include <sys/types.h>

#include "lists.h"

#define PAGE_NONE            0U
#define PAGE_R               1U
#define PAGE_W               2U
#define PAGE_RW              3U
#define PAGE_X               4U
#define PAGE_RX              5U
#define PAGE_RWX             7U
#define PAGE_SHARED          8U
#define PAGE_PRIVATE        16U
#define PAGE_STACK          32U

typedef struct{
    void                    *start;
    size_t                   length;
    unsigned long            offset;
    dev_t                    device;
    ino_t                    inode;
    unsigned char            perms;
    char                     name[];
}page;

extern list blocks;
extern list files;
extern list history;

extern list get_pmap();
void print_colored_pointer(list pmap, void * addr);
extern char * get_page_perms(unsigned char perms);

void shared_vars_init();
void sahred_vars_exit();

#endif //SHARED_VARS_H
