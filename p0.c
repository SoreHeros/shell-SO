#include <stdio.h>
#include <string.h>


#include "p0/echo.h"
#include "p0/pid.h"

#define INPUT_BUFFER_SIZE 1024

typedef const struct{
    char * name;
    void (*function)(char **, int);
    void (*help)();
}command;

void test_function(char* string, int len);
void print_prompt();


command commands[] =
        {
        {"echo",    echo,   echo_help},
        {"pid",     pid,    pid_help},
        };

//imprime el inicio
void print_prompt(){
    printf("\033[92mtemporal:\033[0m");
}

void test_function(char * string, int len){
    fputs(string, stdout);
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

//devuelve el número de tokens
int tokenizer(char ** tokens, char * string){
    int i = 1;

    if((tokens[0]= strtok(string, " \n\t"))==NULL)
        return 0;
    while ((tokens[i] = strtok(NULL, " \n\t")) != NULL)
        i++;
    return i;
}

int main(){
    //init
    int exit = 0;
    char input_buffer[INPUT_BUFFER_SIZE];
    char * tokens[INPUT_BUFFER_SIZE];
    int token_number;

    //sección repetida
    while(!exit){
        print_prompt();


        if(!read_input(input_buffer)){
            printf("\033[91mERROR EN LA LECTURA DE DATOS: ENTRADA DEMASIADO GRANDE\033[0m\n");
            continue;
        }

        token_number = tokenizer(tokens, input_buffer);

        commands[0].function(tokens, token_number);
        //exit = 1;
    }

    //fin
    return 0;
}