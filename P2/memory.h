//
// Created by heros on 30/10/24.
//

#ifndef MALLOCS_H
#define MALLOCS_H
void mallocs_init();
void mallocs_exit();

void allocate(char **, int);
void allocate_help();
void memdump(char **, int);
void memdump_help();
void readfile(char **, int);
void readfile_help();
void process(char **, int);
void process_help();
void memfill(char **, int);
void memfill_help();
void stack(char **, int);
void stack_help();
void pmap(char **, int);
void pmap_help();

//todo
void deallocate(char **, int);
void deallocate_help();
void memory(char **, int);
void memory_help();
void writefile(char **, int);
void writefile_help();
void read(char **, int);
void read_help();
void write(char **, int);
void write_help();
void recurse(char **, int);
void recurse_help();

#endif //MALLOCS_H
