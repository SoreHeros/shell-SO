//
// Created by heros on 19/09/24.
//

#ifndef SHELL_SO_FILES_H
#define SHELL_SO_FILES_H

void open_command(char **, int);
void open_help();
void close_command(char **, int);
void close_help();
void cd(char **, int);
void cd_help();
void dup_command(char **, int);
void dup_help();

//todo
void read_command(char **, int);
void read_help();
void write_command(char **, int);
void write_help();
void execute(char **, int);
void execute_help();

#endif //SHELL_SO_FILES_H
