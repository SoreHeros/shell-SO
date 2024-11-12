//
// Created by heros on 11/11/24.
//

#ifndef FORMAT_H
#define FORMAT_H

#include "lists.h"

typedef union{
    char c;
    unsigned char uc;
    short s;
    unsigned short us;
    int i;
    unsigned int ui;
    long l;
    unsigned long ul;
    void * p;
}integer;

void rand_color_print(char *);

void get_flags(char ** tokens, int flagLen, char * flags[flagLen], int * retFlags[flagLen]);
integer formated_number(char *);



#endif //FORMAT_H
