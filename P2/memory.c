//
// Created by heros on 30/10/24.
//

#include "memory.h"

#include <stdint.h>

#include "../utils/lists.h"
#include "../utils/shared_vars.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/sysmacros.h>
#include <unistd.h>

#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

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
        if(a->type != SHARED)
            free(a->addr);
        else
            printf("WARNING: EXITTING PROCESS WITH OPENED SHARED MEMORY WITH KEY %i", 1234);//todo add key
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

    for(int i = 0; i < len; i++){//todo print mode
        alloc a = list_get(blocks_list, i);
        struct tm * time = localtime(&a->time);
        printf("%04i/%02i/%02i-%02i:%02i:%02i\t", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);
        printf("0x%016lx 0x%08x(%10u) \n", (uintptr_t) a->addr, a->size, a->size);
    }
}

//retrurn the memory block of the allocated memory, -1 if it doesen't exist
int allocatedAt(void * p, unsigned long long int size){
    for(int i = 0; i < list_length(blocks_list); i++){
        alloc a = list_get(blocks_list, i);
        if(a->addr <= p && p + size <= a->addr + a->size)
            return i;
    }
    return -1;
}

void allocateMalloc(unsigned size){
    alloc a = calloc(1, sizeof(struct alloc));
    if(a == NULL){
        perror("ERROR ON ALLOCATE (new list element)");
        return;
    }
    time(&a->time);
    a->addr = calloc(1, size);
    if(a->addr == NULL){
        perror("ERROR ON ALLOCATE (allocating amemory)");
        free(a);
        return;
    }
    //for(int i = 0; i < size; i++)
    //    ((uint8_t *)a->addr)[i] = i;
    a->size = size;
    a->type = MALLOC;
    list_append(blocks_list, a);
    printf("allocated %i bytes on %p\n", size, a->addr);
}


void createShared(int key, long size){
    int id = shmget(key, size, 0777 | IPC_CREAT | IPC_EXCL);
    if(id == -1){
        perror("ERROR ON CREATING SHARED MEMORY:");
        return;
    }
    struct shmid_ds s;

    void * p = shmat(id, NULL, 0);
    alloc a = malloc(sizeof(struct alloc));

    shmctl(id, IPC_STAT, &s);
    a->addr = p;
    a->size = size;
    a->time = s.shm_atime;
    a->type = SHARED;

    list_append(blocks_list, a);
}

void linkShared(int key){
    int id = shmget(key, 0, 0777);
    if(id == -1){
        perror("ERROR ON LINKING SHARED MEMORY:");
        return;
    }
    struct shmid_ds s;

    void * p = shmat(id, NULL, 0);
    alloc a = malloc(sizeof(struct alloc));

    shmctl(id, IPC_STAT, &s);
    a->addr = p;
    a->size = s.shm_segsz;
    a->time = s.shm_atime;
    a->type = SHARED;

    list_append(blocks_list, a);
}

void allocateMap(char * name, char * perms){
    int protection=0;
    if (strlen(perms)<4) {
        if (strchr(perms,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perms,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perms,'x')!=NULL) protection|=PROT_EXEC;
    }

    int df = -1, map=MAP_PRIVATE, modo=O_RDONLY;
    struct stat s;
    void *p;

    if (protection&PROT_WRITE)
        modo=O_RDWR;
    if (stat(name,&s)==-1 || (df=open(name, modo))==-1)
        p = NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
        p = NULL;

    /* Guardar en la lista    InsertarNodoMmap (&L,p, s.st_size,df,fichero); */
    /* Gurdas en la lista de descriptores usados df, fichero*/

    if (p==NULL)
        perror ("Imposible mapear fichero");
    else{
        printf ("fichero %s mapeado en %p\n", name, p);
    }

}

void allocate(char ** tokens, int token_number){//todo allocate -mmap
    if(token_number == 0){
        printMemBlocks();
        return;
    }//todo hacer el print de los bloques específicos

    if(!strcmp(tokens[0], "-malloc")){
        if(token_number < 2){
            fprintf(stderr, "ERROR ON PARSING: -malloc must have a numerical argument after it\n");
            return;
        }
        allocateMalloc(interpretNumberFormat(tokens[1]));
    }else if(!strcmp(tokens[0], "-mmap")){
        if(token_number < 3){
            fprintf(stderr, "ERROR ON PARSING: -mmap must have a file name and perms after it\n");
            return;
        }
        allocateMap(tokens[1], tokens[2]);
    }else if(!strcmp(tokens[0], "-createshared")){
        if(token_number < 3){
            fprintf(stderr, "ERROR ON PARSING: -createshared must have a key and size it\n");
            return;
        }
        createShared(interpretNumberFormat(tokens[1]), interpretNumberFormat(tokens[2]));
    }else if(!strcmp(tokens[0], "-shared")){
        if(token_number < 2){
            fprintf(stderr, "ERROR ON PARSING: -shared must have a key it\n");
            return;
        }
        linkShared(interpretNumberFormat(tokens[1]));
    }else{
        fprintf(stderr, "ERROR ON PARSING: %s isn't a valid flag\n", tokens[1]);
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

void deallocate(char ** tokens, int token_number){

}

void deallocate_help(){
    printf("\tdeallocate [addr] [-malloc n] [-mmap file] [-delkey cl] [-shared  cl]\n");
    printf("empty:\tshows the malloced memory\n");
    printf("addr:\taddress to free (can be decimal, hex(0x), bin(0b), oct(0o))\n");
    printf("n:\tnumber (can be decimal, hex(0x), bin(0b), oct(0o))\n");
    printf("file:\tfile name\n");
    printf("perm:\tfile permissions\n");
    printf("cl:\tshared memory key\n");
    printf("-malloc:\tdeallocates a memory block of the specified size\n");
    printf("-mmap:\tdeallocates the memory asigned to the dump of thee specified file\n");
    printf("-delkey:\tdeletes the specified key\n");
    printf("-shared:\tdeataches the memory block with the specified key\n");
}

void memdumplocal(uint8_t * addr, long long int size){
    list l = get_pmap();
    uint8_t prevBytes[0x10] = {0};
    int isRep = -1;//-1 para que siempre imprima por lo menos el primero
    uintptr_t p;
    for(p = (uintptr_t)addr & ~(uintptr_t)0xf; p < (uintptr_t)(addr + size); p += 0x10){
        uint8_t bytes[0x10] = {0};


        //get bytes
        for(int i = 0; i < 0x10; i++)
            if(!(p + i < (uintptr_t)addr || (uintptr_t)(addr + size) <= p + i))
                bytes[i] = ((uint8_t *)p)[i];

        //print start
        if(isRep >= 0 && !memcmp(bytes, prevBytes, 0x10)){
            if(!isRep){
                isRep = 1;
                printf("[...]\n");
            }
            continue;//todo meter flag
        }
        print_colored_pointer(l, (void *)p);
        printf("     ");
        isRep = 0;

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
            else if(bytes[i] < 32 || 127 <= bytes[i])
                printf(".");
            else
                printf("%c", bytes[i]);

        printf(" ");

        for(int i = 0x8; i < 0x10; i++)
            if(p + i < (uintptr_t)addr || (uintptr_t)(addr + size) <= p + i)
                printf(" ");
            else if(bytes[i] < 32 || 127 <= bytes[i])
                printf(".");
            else
                printf("%c", bytes[i]);
        printf("] ");

        //print pointers
        for(int i = 0; i < 2; i++){
            printf("  ");
            print_colored_pointer(l, ((void **)bytes)[i]);
        }
        printf("\n");
        memcpy(prevBytes, bytes, 0x10);
    }
    if(isRep){
        print_colored_pointer(l, (void *)p);
        printf("\n");
    }
}

void memdump(char ** tokens, int token_number){

    if(token_number == 0){
        printMemBlocks();
        return;
    }


    char safe = 1;
    uint8_t * addr;
    long long int size;
    if(!strcmp(tokens[0], "-unsafe"))
        safe = 0;

    if(safe){
        addr = (uint8_t *)interpretNumberFormat(tokens[0]);
        if(token_number > 1)
            size = interpretNumberFormat(tokens[1]);
        else
            size = 0;
    }else{
        addr = (uint8_t *)interpretNumberFormat(tokens[1]);
        if(token_number > 2)
            size = interpretNumberFormat(tokens[2]);
        else
            size = 0;
    }

    //test if is malloced
    int memblock = allocatedAt(addr, size);
    if(size == 0 && memblock >= 0){
        alloc a = list_get(blocks_list, memblock);
        size = a->addr + a->size - (void *)addr;
    }else if(size == 0){
        size = 8;
    }
    if(memblock == -1 && safe){
        fprintf(stderr, "ERROR ON SAFE MODE: ADDR ISN'T ALLOCATED OR SIZE IS TOO BIG (use -unsafe to ignore)\n");
        return;
    }

    memdumplocal(addr, size);
}
void memdump_help(){
    printf("\tmemdump [-unsafe] addr [size]\n");
    printf("empty:\tshows the malloced memory\n");
    printf("addr:\tthe address of the memory to be dumped (can be decimal, hex(0x), bin(0b), oct(0o))\n");
    printf("size:\tthe number of bytes to be dumped (can be decimal, hex(0x), bin(0b), oct(0o)) (0 for everything untill end of block or 8 bytes depending on safety)\n");
    printf("-unsafe:\tignores the segfault precautions\n");
}

void readfile(char ** tokens, int token_number){

    if(token_number == 0){
        fprintf(stderr, "ERROR ON PARSING: ILLEGAL NUMBER OF ARGUMENTS\n");
        return;
    }

    FILE * f = fopen(tokens[0], "rb");
    if(f == NULL){
        perror("ERROR ON OPENING FILE");
        return;
    }

    uint8_t * addr;
    int size;

    if(token_number == 1){
        //create block of file size
        fseek(f, 0, SEEK_END);
        size = ftell(f) + 1;//+1 for \0
        fseek(f, 0, SEEK_SET);
        allocateMalloc(size);
        addr = ((alloc)list_get(blocks_list, list_length(blocks_list) - 1))->addr;
    }else{
        //fill asigned
        addr = (uint8_t *)interpretNumberFormat(tokens[1]);
        if(token_number > 2){
            size = interpretNumberFormat(tokens[2]);
        }else{
            size = 0;
        }
        int aux;
        if((aux = allocatedAt(addr, size)) == -1){
            fprintf(stderr, "ERROR ON PARSING: ADDR ISN'T ALLOCATED OR SIZE IS TOO BIG\n");
            fclose(f);
            return;
        }
        if(size == 0){
            alloc a = list_get(blocks_list, aux);
            size = a->addr + a->size - (void *)addr;
        }
    }

    for(int i = 0; i < size; i++){
        addr[i] = fgetc(f);
    }

    addr[size - 1] = '\0';

    printf("%i bytes written to %p\n", size, addr);

    fclose(f);
}
void readfile_help(){
    printf("\treadfile filename [addr [size]]\n");
    printf("filename:\tname of the file to be read\n");
    printf("empty:\tallocates the whole file on a new memory block\n");
    printf("addr:\tthe address of the memory to be written (can be decimal, hex(0x), bin(0b), oct(0o))\n");
    printf("size:\tthe number of bytes to be read (can be decimal, hex(0x), bin(0b), oct(0o)) (0 for everything untill end of block)\n");
}

typedef struct{
    void * addr;
    int top;
    int size;
}memstack;

typedef struct{
    memstack * stack;
    int offset;
    int size;
}memstackvar;

memstack initstack(void * addr, int size){
    memstack s;
    s.addr = addr;
    s.size = size;
    s.top = 0;

    //limpiar pantalla
    printf("\33[?1049h");
    printf("\33[H");
    printf("\33[0J");
    memdumplocal(addr, size);

    return s;
}

memstackvar getvar(memstack * s, int align, int size){
    memstackvar v;
    v.stack = s;

    uintptr_t p = (uintptr_t)s->addr + s->top;

    if(p % align){
        int offset = align - (p % align);
        p += offset;
        s->top += offset;
    }

    v.offset = p - (uintptr_t)s->addr;

    if(size < 0)
        size = (s->size - s->top) / align * align;

    s->top += size;
    v.size = size;

    return v;
}

void updatevar(memstack s, void * addr, int size){
    int line = 0, column = 0, columnchar = 0;

    intptr_t top = ((intptr_t)s.addr >> 4) - 1;
    line = ((intptr_t)addr >> 4) - top;

    column = 24 + 3 * ((intptr_t)addr & 0xf);
    columnchar = 79 + ((intptr_t)addr & 0xf);

    if(column >= 47){
        column += 2;
        columnchar++;
    }

    for(int i = 0; i < size; i++){

        if(column >= 47 && column < 50){
            column += 2;
            columnchar++;
        }else if(column >= 74){
            column = 24;
            columnchar = 79;
            line++;
        }

        uint8_t val = ((uint8_t *)addr)[i];

        printf("\33[%i;%iH", line, column);

        printf("%02x", val);


        printf("\33[%i;%iH", line, columnchar);

        if(32 <= val && val < 127)
            printf("%c", ((uint8_t *)addr)[i]);
        else
            printf(".");

        column+= 3;
        columnchar++;
    }
}

void process(char ** tokens, int token_number){

    if(token_number <= 1){
        fprintf(stderr, "ERROR ON PARSING: ILLEGAL NUMBER OF ARGUMENTS\n");
        return;
    }

    char * proc = tokens[0];
    void * addr = (void *)interpretNumberFormat(tokens[1]);
    int size = 0;

    if(token_number > 2)
        size = interpretNumberFormat(tokens[2]);

    int memblock = allocatedAt(addr, size);

    if(memblock == -1){
        fprintf(stderr, "ERROR ON PARSING: ADDR ISN'T ALLOCATED OR SIZE IS TOO BIG\n");
        return;
    }

    if(size == 0){
        alloc a = list_get(blocks_list, memblock);
        size = a->addr + a->size - (void *)addr;
    }

    memstack s = initstack(addr, size);
    memstackvar stacki = getvar(&s, __alignof(int), sizeof(int));
    memstackvar stackj = getvar(&s, __alignof(int), sizeof(int));
    memstackvar stacknum = getvar(&s, __alignof(int), sizeof(int));
    memstackvar stackarr = getvar(&s, __alignof(short int), -1);
    int * i = stacki.offset + s.addr;
    int * j = stackj.offset + s.addr;
    int * num = stacknum.offset + s.addr;
    short int * arr = stackarr.offset + s.addr;
    int arr_size = stackarr.size / sizeof(*arr);

    //ver tamaño mínimo
    if(arr_size <= 0){
        fprintf(stderr, "ERROR ON PROCESSING: SPACE ISN'T BIG ENOUGH\n");
        return;
    }

    arr[0] = 2;
    updatevar(s, &arr[0], sizeof(*arr));

    for(*i = 1, *num = 3; *i < arr_size; (*i)++, (*num)++){
        updatevar(s, i, sizeof(*i));
        updatevar(s, num, sizeof(*num));
        int prime = 1;
        for(*j = *i - 1; *j >= 0; (*j)--){
            updatevar(s, j, sizeof(*j));
            if(!(*num % arr[*j])){
                prime = 0;
                break;
            }
        }
        if(prime){
            arr[*i] = *num;
            updatevar(s, &arr[*i], sizeof(*arr));
        }else{
            (*i)--;
            updatevar(s, i, sizeof(*i));
        }
    }
    printf("\33[?1049l");
    memdumplocal(addr, size);
}
void process_help(){
    printf("\tprocess prime|newton|sort addr [size]\n");
    printf("addr:\tthe address of the memory to be used (can be decimal, hex(0x), bin(0b), oct(0o))\n");
    printf("empty:\tuses the full block\n");
    printf("size:\tthe number of bytes to be used (can be decimal, hex(0x), bin(0b), oct(0o)) (0 for everything untill end of block)\n");
}

void memfill(char ** tokens, int token_number){
    if(token_number < 2){
        fprintf(stderr, "ERROR ON PARSING: ILLEGAL NUMBER OF ARGUMENTS\n");
        return;
    }

    uint8_t * addr = (uint8_t *)interpretNumberFormat(tokens[0]);
    int size = 0;
    char value;

    if(token_number >= 3)
        size = interpretNumberFormat(tokens[1]);

    if(tokens[token_number - 1][0] == '\''){
        value = tokens[token_number - 1][1];
    }else{
        value = interpretNumberFormat(tokens[token_number - 1]);
    }

    int memblock = allocatedAt(addr, size);

    if(memblock == -1){
        fprintf(stderr, "ERROR ON PARSING: ADDR ISN'T ALLOCATED OR SIZE IS TOO BIG\n");
        return;
    }

    if(size == 0){
        alloc a = list_get(blocks_list, memblock);
        size = a->addr + a->size - (void *)addr;
    }

    for(int i = 0; i < size; i++)
        addr[i] = value;
}
void memfill_help(){
    printf("\tmemfill addr [size] char|number\n");
    printf("addr:\tthe address of the memory to be written (can be decimal, hex(0x), bin(0b), oct(0o))\n");
    printf("empty:\tuses the full block\n");
    printf("size:\tthe number of bytes to be written (can be decimal, hex(0x), bin(0b), oct(0o)) (0 for everything untill end of block)\n");
    printf("char:\tthe char to fill the bytes (must be between '')\n");
    printf("number:\tthe number to fill the bytes with (can be decimal, hex(0x), bin(0b), oct(0o)) (0 for everything untill end of block)\n");
}

void stack(char ** tokens, int token_number){
    if(token_number == 1){
        fprintf(stderr, "ERROR ON PARSING: ILLEGAL NUMBER OF ARGUMENTS\n");
        return;
    }

    const char stack[] = ">STACK<";



    if(token_number == 0){
        printf("%p\n", &stack);
        return;
    }

    int low = interpretNumberFormat(tokens[0]);
    int high= interpretNumberFormat(tokens[1]);

    memdumplocal((void *)&stack - low, low + high);
}
void stack_help(){
    printf("\tstack inf sup\n");
    printf("inf:\tthe number of bytes to be displayed under the current stack pointer (can be decimal, hex(0x), bin(0b), oct(0o))\n");
    printf("sup:\tthe number of bytes to be displayed above the current stack pointer (can be decimal, hex(0x), bin(0b), oct(0o))\n");
}

//updates the list l with the current process pages, if l is null it will create a new list, returns the process page list


void pmap_command(char **, int){
    list l = get_pmap();

    for(int i = 0; i < list_length(l); i++){
        page * p = list_get(l, i);
        print_colored_pointer(l, p->start);
        printf(": 0x%08lx %s %s\n", p->length, get_page_perms(p->perms), p->name);
    }
}
void pmap_help(){
    printf("\tpmap\n");
}


void print(char ** tokens, int token_number){
    char * addr = (char *)interpretNumberFormat(tokens[0]);
    if(token_number > 1){
        int size = interpretNumberFormat(tokens[1]);
        for(int i = 0; i < size; i++)
            putchar(addr[i]);
    }else{
        for(;*addr!='\0';addr++)
            putchar(*addr);
    }
    putchar('\n');
}
void print_help(){
    printf("\tprint addr [size]\n");
    printf("addr:\tmem dir to print\n");
    printf("empty:\teverything untill '\\0'\n");
    printf("size:\tignores '\\0' and prints size number of characters\n");
}