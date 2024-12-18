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
#include <fcntl.h>

#include <sys/types.h>
#include <unistd.h>

#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

typedef enum{
    ANY = 0b111, MALLOC = 0b1, SHARED = 0b10, MAPPED = 0b100
}alloc_type;

typedef struct alloc{
    void * addr;
    time_t time;
    size_t size;
    alloc_type type;
    union{
        int key;
        char name[0];//0 para engañar al compilador con que es de tamaño variable
    };
} * alloc;

list blocks_list;

void mallocs_init(){
    blocks_list = list_init();
}
void mallocs_exit(){
    for(int i = 0; i < list_length(blocks_list); i++){
        alloc a = list_get(blocks_list, i);
        switch (a->type){
        case MALLOC:
            free(a->addr);
            break;
        case SHARED:
            printf("\33[31mWARNING\33[0m: EXITTING PROCESS WITH OPENED SHARED MEMORY WITH KEY %i\n", a->key);
            break;
        default:
            break;
        }
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

void printMemBlocks(alloc_type t){
    int len = list_length(blocks_list);

    int printed = 0;
    for(int i = 0; i < len; i++){
        alloc a = list_get(blocks_list, i);
        if(a->type & t){
            struct tm * time = localtime(&a->time);
            printf("%04i/%02i/%02i-%02i:%02i:%02i\t", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);
            printf("0x%016lx 0x%08lx(%10lu) ", (uintptr_t) a->addr, a->size, a->size);
            switch (a->type){
                case ANY:
                    printf("ANY    ");
                    break;
                case MALLOC:
                    printf("MALLOC ");
                    break;
                case SHARED:
                    printf("SHARED KEY:  %i", a->key);
                    break;
                case MAPPED:
                    printf("MAPPED NAME: %s", a->name);
                    break;
            }
            printf("\n");
            printed++;
        }
    }

    if(printed == 0){
        printf("There aren't any blocks with type: ");
        switch (t){
            case ANY:
                printf("ANY    ");
                break;
            case MALLOC:
                printf("MALLOC ");
                break;
            case SHARED:
                printf("SHARED ");
                break;
            case MAPPED:
                printf("MAPPED ");
                break;
        }
        printf("\n");
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
        perror("ERROR ON CREATING SHARED MEMORY");
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
    a->key  = key;

    list_append(blocks_list, a);
    printf("allocated %li bytes on %p for shared memory with key %i\n", a->size, a->addr, a->key);
}

void linkShared(int key){
    int id = shmget(key, 0, 0777);
    if(id == -1){
        perror("ERROR ON LINKING SHARED MEMORY");
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
    a->key  = key;

    list_append(blocks_list, a);
    printf("linked %li bytes on %p for shared memory with key %i\n", a->size, a->addr, a->key);
}

void allocateMap(char * name, char * perms){
    int protection=0;
    if (strlen(perms)<4) {
        if (strchr(perms,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perms,'w')!=NULL) protection|=PROT_WRITE | PROT_READ;
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

    if (p==NULL)
        perror ("Imposible mapear fichero");
    else{
        printf ("fichero %s mapeado en %p\n", name, p);

        alloc a = malloc(sizeof(struct alloc) + strlen(name) + 1);

        a->addr = p;
        a->size = s.st_size;
        time(&a->time);
        a->type = MAPPED;
        strcpy(a->name, name);

        list_append(blocks_list, a);
        close(df);
    }

}

void allocate(char ** tokens, int token_number){
    if(token_number == 0){
        printMemBlocks(ANY);
        return;
    }

    if(token_number == 1){
        if(!strcmp(tokens[0], "-malloc"))
            printMemBlocks(MALLOC);
        else if(!strcmp(tokens[0], "-mmap"))
            printMemBlocks(MAPPED);
        else if(!strcmp(tokens[0], "-shared") || !strcmp(tokens[0], "-createshared"))
            printMemBlocks(SHARED);
        else
            fprintf(stderr ,"ERROR ON PARSING: %s isn't a valid flag", tokens[0]);
        return;
    }

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
        fprintf(stderr, "ERROR ON PARSING: %s isn't a valid flag\n", tokens[0]);
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

void deallocateAddr(void * addr){
    alloc a = list_get(blocks_list, allocatedAt(addr, 0));
    switch(a->type){
    case MALLOC:
        printf("freeing mallocd memory at %p of size %li\n", a->addr, a->size);
        free(a->addr);
        break;
    case MAPPED:
        printf("unmapping file %s at %p\n", a->name, a->addr);
        munmap(a->addr, a->size);
        break;
    case SHARED:
        printf("detaching shared memory with key %i at %p\n", a->key, a->addr);
        shmdt(a->addr);
        break;
    default:
        break;
    }
}

void deallocate(char ** tokens, int token_number){
    if(token_number == 0){
        printMemBlocks(ANY);
        return;
    }

    if(token_number == 1){
        int pos = allocatedAt((void *)interpretNumberFormat(tokens[0]), 0);
        if(pos < 0){
            fprintf(stderr,"ERROR ON FREEING: no allocated block on 0x%016lx exists\n", (unsigned long)interpretNumberFormat(tokens[0]));
            return;
        }

        alloc a = list_get(blocks_list, pos);
        deallocateAddr(a->addr);

        list_remove(blocks_list, pos);
        free(a);

        return;
    }

    if(!strcmp(tokens[0], "-malloc")){
        if(token_number < 2){
            fprintf(stderr, "ERROR ON PARSING: -malloc must have a numerical argument after it\n");
            return;
        }
        unsigned long size = interpretNumberFormat(tokens[1]);
        for(int i = 0; i < list_length(blocks_list); i++){
            alloc a = list_get(blocks_list, i);
            if(a->size == size && a->type == MALLOC){
                deallocateAddr(a->addr);
                list_remove(blocks_list, i);
                free(a);
                return;;
            }
        }
        fprintf(stderr,"ERROR ON FREEING: no mallocd block of size %li exists\n", size);
    }else if(!strcmp(tokens[0], "-mmap")){
        if(token_number < 2){
            fprintf(stderr, "ERROR ON PARSING: -mmap must have a file name\n");
            return;
        }
        char * name = tokens[1];
        for(int i = 0; i < list_length(blocks_list); i++){
            alloc a = list_get(blocks_list, i);
            if(a->type == MAPPED && !strcmp(a->name, name)){
                deallocateAddr(a->addr);
                list_remove(blocks_list, i);
                free(a);
                return;;
            }
        }
        fprintf(stderr,"ERROR ON FREEING: no mapped file with name %s exists\n", name);
    }else if(!strcmp(tokens[0], "-delkey")){
        if(token_number < 2){
            fprintf(stderr, "ERROR ON PARSING: -delkey must have a key\n");
            return;
        }
        int key = interpretNumberFormat(tokens[1]);
        int ret = shmctl(shmget(key, 0, 0), IPC_RMID, NULL);
        if(ret == -1)
            perror("ERROR ON DELETING KEY");
    }else if(!strcmp(tokens[0], "-shared")){
        if(token_number < 2){
            fprintf(stderr, "ERROR ON PARSING: -shared must have a key it\n");
            return;
        }
        int key = interpretNumberFormat(tokens[1]);
        for(int i = 0; i < list_length(blocks_list); i++){
            alloc a = list_get(blocks_list, i);
            if(a->type == SHARED && a->key == key){
                deallocateAddr(a->addr);
                list_remove(blocks_list, i);
                free(a);
                return;;
            }
        }
        fprintf(stderr,"ERROR ON FREEING: no linked shared memory with key %i exists\n", key);
    }else{
        fprintf(stderr, "ERROR ON PARSING: %s isn't a valid flag\n", tokens[0]);
    }
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
            continue;
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
        printMemBlocks(ANY);
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
        if(feof(f)){
            addr[i] = '\0';
            printf("EOF reached at %i bytes\n", i);
            break;
        }
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

    memset(addr, value, size);
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

void localRecurse(int left){
    if(left < 0)
        return;
    static int recursions = 0;
    static uint8_t staticArr[2048];
    uint8_t autoArr[2048];

    recursions++;
    if(recursions >= 2000){
        fprintf(stderr,"ERROR ON RECURSE: too many recursions\n");
        return;
    }
    printf("recursions left:%5i(%p) array %p estatico %p\n", left, &left, &autoArr, &staticArr);
    localRecurse(left - 1);
    recursions = 0;
}

void recurse(char ** tokens, int token_number){
    int size = 25;
    if(token_number > 0)
        size = interpretNumberFormat(tokens[0]);
    localRecurse(size);
}
void recurse_help(){
    printf("\trecurse n\n");
    printf("n:\tnumber of times to recuse\n");
}

void memory(char ** tokens, int token_number){
    struct{
        char funcs;
        char vars;
        char blocks;
        char pmap;
    }flags = {0};

    for(int i = 0; i < token_number; i++){
        if(!strcmp(tokens[i], "-funcs"))
            flags.funcs = 1;
        else if(!strcmp(tokens[i], "-blocks"))
            flags.blocks = 1;
        else if(!strcmp(tokens[i], "-vars"))
            flags.vars = 1;
        else if(!strcmp(tokens[i], "-pmap"))
            flags.pmap = 1;
        else if(!strcmp(tokens[i], "-all")){
            flags.vars = 1;
            flags.blocks = 1;
            flags.funcs = 1;
        }
    }

    if(!(flags.blocks || flags.funcs || flags.pmap || flags.vars)){
        flags.blocks = 1;
        flags.funcs = 1;
        flags.vars = 1;
    }

    if(flags.funcs){
        printf("\nfunciones del programa:\n");
        printf("funcion memory: \t%p\n", memory);
        printf("funcion recurse:\t%p\n", recurse);
        printf("funcion print:  \t%p\n", print);

        printf("\nfunciones de librerias:\n");
        printf("funcion printf: \t%p\n", printf);
        printf("funcion strcpy: \t%p\n", strcpy);
        printf("funcion time:   \t%p\n", time);
    }

    if(flags.vars){
        printf("\nvariables:\n");
        printf("External:            \t%16p, %16p, %16p\n", &var1, &var2, &var3);
        printf("External initialized:\t%16p, %16p, %16p\n", &blocks, &files, &history);

        static int var1, var2, var3;
        printf("Static:              \t%16p, %16p, %16p\n", &var1, &var2, &var3);


        static int var4 = 1, var5 = 2, var6 = 3;
        printf("Static initialized:  \t%16p, %16p, %16p\n", &var4, &var5, &var6);

        int var7 = 1;
        float var8 = 1;
        char var9 = '1';
        printf("automatic:           \t%16p, %16p, %16p\n", &var7, &var8, &var9);
    }

    if(flags.blocks){
        printf("\nmemory blocks:\n");
        printMemBlocks(ANY);
    }

    if(flags.pmap){
        printf("\nprocess map:\n");
        pmap_command(NULL, 0);
    }
}
void memory_help(){
    printf("\tmemory -funcs|-vars|-blocks|-all|-pmap ...\n");
    printf("-funcs:\tprints the addresses of 3 program and 3 library functions\n");
    printf("-vars:\tprints the addresses of 3 external, 3 external initialized, 3 static, 3 static initialized and 3 automatic variables\n");
    printf("-blocks:\tprints the addresses of all allocated blocks\n");
    printf("-all:\tsame ass -funcs -vars -blocks\n");
    printf("-pmap:\tsame as calling pmap\n");
}

void writefile(char ** tokens, int token_number){
    if(token_number < 3){
        fprintf(stderr, "ERROR ON PARSING: writefile needs 3 argumens\n");
        return;
    }

    FILE * f = fopen(tokens[0], "wb");
    if(f == NULL){
        perror("ERROR ON OPENING FILE");
        return;
    }

    uint8_t * addr = (void *)interpretNumberFormat(tokens[1]);
    size_t size = interpretNumberFormat(tokens[2]);

    int pos = allocatedAt(addr, size);
    if(pos < 0){
        fprintf(stderr, "ERROR ON PARSING: addr or size is too big or isnt allocated\n");
        fclose(f);
        return;
    }

    fwrite(addr, 1, size, f);
    fclose(f);
}
void writefile_help(){
    printf("\twritefile file addr size\n");
    printf("file:\tname of the file to write\n");
    printf("addr:\taddress to read from\n");
    printf("size:\tnº of bytes to read\n");
}

void write_command(char ** tokens, int token_number){
    if(token_number < 3){
        fprintf(stderr, "ERROR ON PARSING: writefile needs 3 argumens\n");
        return;
    }

    FILE * f = fdopen(interpretNumberFormat(tokens[0]), "wb");
    if(f == NULL){
        perror("ERROR ON OPENING FILE");
        return;
    }

    uint8_t * addr = (void *)interpretNumberFormat(tokens[1]);
    size_t size = interpretNumberFormat(tokens[2]);

    int pos = allocatedAt(addr, size);
    if(pos < 0){
        fprintf(stderr, "ERROR ON PARSING: addr or size is too big or isnt allocated\n");
        fclose(f);
        return;
    }
    fwrite(addr, 1, size, f);
    fclose(f);
}
void write_help(){
    printf("\twrite df addr size\n");
    printf("df:\tfile descriptor of the file to write\n");
    printf("addr:\taddress to read from\n");
    printf("size:\tnº of bytes to read\n");
}

void read_command(char ** tokens, int token_number){

    if(token_number == 0){
        fprintf(stderr, "ERROR ON PARSING: ILLEGAL NUMBER OF ARGUMENTS\n");
        return;
    }

    FILE * f = fdopen(interpretNumberFormat(tokens[0]), "rb");
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
        if(feof(f)){
            addr[i] = '\0';
            printf("EOF reached at %i bytes\n", i);
            break;
        }
    }

    addr[size - 1] = '\0';

    printf("%i bytes written to %p\n", size, addr);

    fclose(f);
}
void read_help(){
    printf("\tread df [addr [size]]\n");
    printf("df:\tfile descriptor of the file to be read\n");
    printf("empty:\tallocates the whole file on a new memory block\n");
    printf("addr:\tthe address of the memory to be written (can be decimal, hex(0x), bin(0b), oct(0o))\n");
    printf("size:\tthe number of bytes to be read (can be decimal, hex(0x), bin(0b), oct(0o)) (0 for everything untill end of block)\n");
}