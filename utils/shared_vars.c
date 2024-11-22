//
// Created by heros on 20/11/24.
//

#include "shared_vars.h"

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysmacros.h>

list blocks;
list files;
list history;
list pmap;

list pid_pages(int pid, list l){

    if(l == NULL){
        l = list_init();
    }else{
        //empty list
        while(list_length(l)){
            free(list_pop(l));
        }
    }

    size_t         size = 0;
    FILE          *maps_file;
    char          *line = NULL;

    if (pid > 0) {
        char path[128];
        int  pathlen;

        pathlen = snprintf(path, sizeof(path), "/proc/%ld/maps", (long)pid);
        if (pathlen < 12) {
            errno = EINVAL;
            return NULL;
        }

        maps_file = fopen(path, "r");
    } else
        maps_file = fopen("/proc/self/maps", "r");

    if (!maps_file)
        return NULL;

    while (getline(&line, &size, maps_file) > 0) {
        page *curr;
        char           perms[8];
        unsigned int   devmajor, devminor;
        unsigned long  addr_start, addr_end, offset, inode;
        int            name_start = 0;
        int            name_end = 0;

        if (sscanf(line, "%lx-%lx %7s %lx %u:%u %lu %n%*[^\n]%n",&addr_start, &addr_end, perms, &offset, &devmajor, &devminor, &inode, &name_start, &name_end) < 7) {
            fclose(maps_file);
            free(line);
            errno = EIO;
            return NULL;
        }

        if (name_end <= name_start)
            name_start = name_end = 0;

        curr = malloc(sizeof(page) + (size_t)(name_end - name_start) + 1);
        if (!curr) {
            fclose(maps_file);
            free(line);
            errno = ENOMEM;
            return NULL;
        }

        if (name_end > name_start)
            memcpy(curr->name, line + name_start, name_end - name_start);
        curr->name[name_end - name_start] = '\0';

        curr->start = (void *)addr_start;
        curr->length = addr_end - addr_start;
        curr->offset = offset;
        curr->device = makedev(devmajor, devminor);
        curr->inode = inode;

        curr->perms = 0U;

        if (strchr(perms, 'r'))
            curr->perms |= PAGE_R;
        if (strchr(perms, 'w'))
            curr->perms |= PAGE_W;
        if (strchr(perms, 'x'))
            curr->perms |= PAGE_X;
        if (strchr(perms, 's'))
            curr->perms |= PAGE_SHARED;
        if (strchr(perms, 'p'))
            curr->perms |= PAGE_PRIVATE;
        if(!strcmp(curr->name, "[stack]"))
            curr->perms |= PAGE_STACK;

        list_append(l, curr);
    }

    free(line);

    if (!feof(maps_file) || ferror(maps_file)) {
        fclose(maps_file);
        errno = EIO;
        return NULL;
    }
    if (fclose(maps_file)) {
        errno = EIO;
        return NULL;
    }

    errno = 0;

    return l;
}

list get_pmap(){
    return pid_pages(0, pmap);
}

page * get_pointer_page(void * p, list l){
    for(int i = 0; i < list_length(l); i++){
        page * pg = list_get(l, i);
        if(pg->start <= p && p < pg->start + pg->length)
            return pg;
    }
    return NULL;
}

extern char * get_page_perms(unsigned char perms){
    static char res[5];
    res[0] = '-';
    res[1] = '-';
    res[2] = '-';
    res[3] = '-';
    res[4] = '\0';

    if(perms & PAGE_R)
        res[0] = 'r';
    if(perms & PAGE_W)
        res[1] = 'w';
    if(perms & PAGE_X)
        res[2] = 'x';
    if(perms & PAGE_SHARED)
        res[3] = 's';

    return res;
}

void print_colored_pointer(list l, void * addr){
    char code[8] = "\033[90m";
    page * pg = get_pointer_page(addr, l);

    //CODIGO COLORES:
    //NULL      ---
    //---       ---
    //r--       r--
    //-w-       -g-
    //--x       --b
    //shared    -gb //-wx nunca va a pasar
    //stack     -g- //-w- nunca va a pasar

    if(pg != NULL){

        code[3] = '0' + (pg->perms & 0b111);

        if(pg->perms & PAGE_SHARED)
            code[3] = '6';

        if(pg->perms & PAGE_STACK)
            code[3] = '2';
    }

    printf("%s0x%016lx\33[0m",code, (intptr_t)addr);
}

void files_init(){
    files = list_init();

    file *f1 = malloc(sizeof(file));

    f1->name = strdup("stdin");
    f1->fd = stdin->_fileno;

    list_append(files, f1);

    file *f2 = malloc(sizeof(file));

    f2->name = strdup("stdout");
    f2->fd = stdout->_fileno;

    list_append(files, f2);

    file *f3 = malloc(sizeof(file));

    f3->name = strdup("stderr");
    f3->fd = stderr->_fileno;

    list_append(files, f3);
}

void shared_vars_init(){
    pmap = list_init();
    files_init();
}

void files_exit(){
    for(int i = 0; i < list_length(files); i++){
        file * f = list_get(files, i);
        close(f->fd);
        free(f->name);
        free(f);
    }

    list_free(files);
}

void sahred_vars_exit(){
    while(list_length(pmap)){
        free(list_pop(pmap));
    }
    list_free(pmap);
    files_exit();
}