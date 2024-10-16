//
// Created by heros on 24/09/24.
//

#ifndef SHELL_SO_COMMAND_MANAGER_H
#define SHELL_SO_COMMAND_MANAGER_H

typedef enum {EXIT, NORMAL, NOTFOUND}command_directive;
#define TOKEN_BUFFER_SIZE 128
#define INPUT_BUFFER_SIZE 1024
#define PROMPT "terminal:"

typedef const struct{
    char name[12];
    command_directive code;
    void (* command)(char **, int);
    void (* help)();
}command_entry;

void command_manager_init();
void command_manager_exit();
command_entry get_command(char * command_name);
void history_append(char * entry);
void history_pop();
int history_len();
char * history_get(int pos);
int tokenize(char ** tokens, char * input);
void print_prompt(char * prompt);
int read_input(char * string);


#endif //SHELL_SO_COMMAND_MANAGER_H
