//
// Created by heros on 24/09/24.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "command_manager.h"
#include "lists.h"
#include "basic_functions.h"
#include "sys_proc_info.h"
#include "files.h"

void help(char **, int);
void help_help();

void historic(char **, int);
void historic_help();

void not_found(char **, int);
void not_found_help();

command_entry nf = {"not found\0", NOTFOUND, not_found, not_found_help};
command_entry commands[] =
        {
                     {"quit\0",      EXIT,          quit,          quit_help},
                     {"exit\0",      EXIT,          exit_command,  exit_help},
                     {"bye\0",       EXIT,          bye,           bye_help},
                     {"help\0",      NORMAL,        help,          help_help},
                     {"authors\0",   NORMAL,        authors,       authors_help},
                     {"echo\0",      NORMAL,        echo,          echo_help},
                     {"pid\0",       NORMAL,        pid,           pid_help},
                     {"ppid\0",      NORMAL,        ppid,          ppid_help},
                     {"infosys\0",   NORMAL,        infosys,       infosys_help},
                     {"historic\0",  NORMAL,        historic,      historic_help},
                    {"open\0",      NORMAL, open_command,  open_help},
                    {"close\0",     NORMAL,         close_command, close_help},
                    {"date\0",      NORMAL,         date,          date_help},
                    {"cd\0",        NORMAL,         cd,            cd_help},
                    {"dup\0",       NORMAL,         dup_command,   dup_help},

        };
command_entry ** commands_pointer;
int commands_len = sizeof(commands) / sizeof(command_entry);
list historial;

//todo fix recursions
#define RECURSION_LIMIT 10
int recursions = 0;

void historic_execute(int n){

    recursions++;
    if(recursions > RECURSION_LIMIT){
        fprintf(stderr, "ERROR AL EJECUTAR UN HISTORICO: DEMASIADAS RECURSIONES\n");
        return;
    }

    if(n < 0 || n >= list_length(historial)){
        fprintf(stderr, "ERROR AL EJECUTAR UN HISTORICO: POSICION NO VALIDA\n");
        return;
    }

    char * str = strdup(list_get(historial, n));
    printf("executing command %i: %s", n, str);
    char * tokens[TOKEN_BUFFER_SIZE];
    int token_number = tokenize(tokens, str);
    command_entry comd = get_command(tokens[0]);
    comd.command(&tokens[1], token_number - 1);
    free(str);
}

void historic(char ** tokens, int token_number){

    int historic_start = 0;

    if(token_number > 0){
        if(tokens[0][0] == '-')
            historic_start = list_length(historial) - atoi(&tokens[0][1]);
        else{
            historic_execute(atoi(tokens[0]));
            recursions = 0;
            return;
        }
    }

    for(int i = historic_start, len = list_length(historial); i < len; i++)
        printf("%3i: %s", i,(char *)list_get(historial, i));
}
void historic_help(){
    printf("\thistoric [N | -N]\nempty:\tprints every command you made\nN:\trepeats command N from the historic\n-N:\tprints the last N commands\n");
}

void help(char ** tokens, int token_number){
    if(token_number == 0) {
        for (int i = 0; i < commands_len; i++) {
            printf("\n");
            commands_pointer[i]->help();
        }
    }else{
        //optimizar?
        for (int i = 0; i < commands_len; i++)
            for(int j = 0; j < token_number; j++)
                if(strcmp(tokens[j], commands[i].name) == 0) {
                    printf("\n");
                    commands_pointer[i]->help();
                }
    }
}
void help_help(){
    printf("\thelp [cmd]\nempty:\tprints every command's help\ncmd:\tprints the comand's help\n");
}

void not_found(char **, int){
    fprintf(stderr, "ERROR AL BUSCAR COMANDO: NO ENCONTRADO\n");
}
void not_found_help(){
    fprintf(stderr, "ERROR AL BUSCAR HELP: NO ENCONTRADO\n");
}

int command_comparator(const void * p1, const void * p2){
    command_entry * comm1 = *(command_entry **)p1, * comm2 = *(command_entry **)p2;
    return strcmp(comm1->name, comm2->name);
}
int bsearch_comparator(const void * key, const void * data){
    command_entry * comm = *(command_entry **)data;
    return strcmp(*(char **)key, comm->name);
}


void command_manager_init(){
    historial = list_init();
    commands_pointer = malloc(commands_len * sizeof(command_entry *));
    for(int i = 0; i < commands_len; i++)
        commands_pointer[i] = &commands[i];
    qsort(commands_pointer, commands_len, sizeof(command_entry *), command_comparator);
    files_init();
}
void command_manager_exit(){
    for(int i = 0; i < list_length(historial); i++)
        free(list_get(historial, i));
    list_free(historial);
    free(commands_pointer);
    files_exit();
}
command_entry get_command(char * command_name){
    command_entry ** comm = (command_entry **) bsearch(&command_name, commands_pointer,commands_len, sizeof(command_entry *), bsearch_comparator);
    if(comm == NULL)
        return nf;
    return **comm;
}
void history_append(char * entry){
    list_append(historial, strdup(entry));
}
void history_pop(){
    free(list_pop(historial));
}
int tokenize(char ** tokens, char * string){
    int i = 1;

    if((tokens[0] = strtok(string, " \n\t"))==NULL)
        return 0;
    while (i < TOKEN_BUFFER_SIZE && (tokens[i] = strtok(NULL, " \n\t")) != NULL)
        i++;

    if(i >= TOKEN_BUFFER_SIZE)
        i = -1;


    return i;
}