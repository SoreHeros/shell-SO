//
// Created by heros on 19/09/24.
//

#ifndef SHELL_SO_FILES_H
#define SHELL_SO_FILES_H

void files_init();
void files_exit();

void open(char **, int);
void open_help();
void close(char **, int);
void close_help();

//todo
void cd(char **, int);
void cd_help();
void dup(char **, int);
void dup_help();
void create(char **, int);
void create_help();
void read(char **, int);
void read_help();
void write(char **, int);
void write_help();
void execute(char **, int);
void execute_help();

#endif //SHELL_SO_FILES_H
