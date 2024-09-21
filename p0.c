#include <stdio.h>
#include <string.h>

#include "p0/basic_functions.h"
#include "p0/sys_proc_info.h"

#define INPUT_BUFFER_SIZE 1024
#define TOKEN_BUFFER_SIZE 128

void help(char **, int);
void help_help();

//todo
void historic(char **, int);
void historic_help();



const struct{
    char * name;
    void (*function)(char **, int);
    void (*help)();
}commands[] =
        {
                {"quit",         quit,         quit_help},
                {"exit",         exit_command, exit_help},
                {"bye",          bye,          bye_help},
                {"help",         help,         help_help},
                {"authors",      authors,      authors_help},
                {"echo",         echo,         echo_help},
                {"pid",          pid,          pid_help},
                {"ppid",         ppid,         ppid_help},
        };

//imprime el inicio
void print_prompt(){
    printf("\033[92mtemporal:\033[0m");
}

//lee la entrada y la procesa en tokens, devuelve 1, si la entrada es demasiado grande devuelve 0
int read_input(char * string){
    fgets(string, INPUT_BUFFER_SIZE, stdin);
    if(strlen(string) == INPUT_BUFFER_SIZE - 1 && string[INPUT_BUFFER_SIZE - 2] != '\n'){
        //devolver error si se ha superado el límite de tamaño (excepto si justo es del tamaño adecuado)
        fseek(stdin, 0, SEEK_END);
        return 0;
    }
    return 1;
}

//devuelve el número de tokens, -1 si se ha superado el límite
int tokenizer(char ** tokens, char * string){
    int i = 1;

    if((tokens[0] = strtok(string, " \n\t"))==NULL)
        return 0;
    while (i < TOKEN_BUFFER_SIZE && (tokens[i] = strtok(NULL, " \n\t")) != NULL)
        i++;

    if(i >= TOKEN_BUFFER_SIZE)
        i = -1;


    return i;
}

enum{EXIT, NORMAL, HISTORIC};

//interpreta los comandos y devuelve un enum
int interpretar(char ** tokens, int token_number){
    if(token_number < 1)
        return NORMAL;

    for(int i = 0; i < (sizeof(commands) / sizeof(commands[0])); i++) //todo quizás busqueda binaria
        if(strcmp(tokens[0], commands[i].name) == 0){
            commands[i].function(&tokens[1], token_number - 1);
            break;
        }

    if(!strcmp(tokens[0], "exit") || !strcmp(tokens[0], "quit") || !strcmp(tokens[0], "bye"))
        return EXIT;

    return NORMAL;
}

void help(char ** tokens, int token_number){
    if(token_number == 0) {
        for (int i = 0; i < (sizeof(commands) / sizeof(commands[0])); i++)
            commands[i].help();
    }else{
        //todo optimizar?
        for (int i = 0; i < (sizeof(commands) / sizeof(commands[0])); i++)
            for(int j = 0; j < token_number; j++)
                if(strcmp(tokens[j], commands[i].name) == 0)
                    commands[i].help();
    }
}
void help_help(){
    printf("\thelp [cmd]\nempty:\tprints every command's help\ncmd:\tprints the comand's help\n");
}

int main(){
    //init
    int interpreter_code = NORMAL;
    char input_buffer[INPUT_BUFFER_SIZE] = {0};
    char * tokens[TOKEN_BUFFER_SIZE] = {NULL};
    int token_number;

    //sección repetida
    while(interpreter_code != EXIT){
        print_prompt();

        if(interpreter_code == HISTORIC){
            //todo substituir el input con un histórico
        }else if(!read_input(input_buffer)){
            printf("\033[91mERROR EN LA LECTURA DE DATOS: ENTRADA DEMASIADO GRANDE\033[0m\n");
            //todo añadir al histórico?
            continue;
        }

        //todo añadir al histórico

        if((token_number = tokenizer(tokens, input_buffer)) == -1){
            printf("\033[91mERROR EN LA CONVERSION A TOKENS: DEMASIADOS TOKENS\033[0m\n");
            continue;
        }else if(!token_number)
            continue;

        interpreter_code = interpretar(tokens, token_number);
    }

    //fin
    return 0;
}
