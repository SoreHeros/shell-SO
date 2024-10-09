//Heros Vicente González heros.vicente@udc.es
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <termios.h>
#include <curses.h>

#include "p0/command_manager.h"

#define INPUT_BUFFER_SIZE 1024

//imprime el inicio
void print_prompt(){
    printf("\033[92mtemporal:\033[0m");
}

//lee la entrada y la procesa en tokens, devuelve 1, si la entrada es demasiado grande devuelve 0
int read_input(char * string){
    fgets(string, INPUT_BUFFER_SIZE, stdin);
    if(strlen(string) == INPUT_BUFFER_SIZE - 1 && string[INPUT_BUFFER_SIZE - 2] != '\n'){
        //devolver error si se ha superado el límite de tamaño (excepto si justo es del tamaño adecuado)
        while (fgetc(stdin) != '\n');
        return 0;
    }
    return 1;
}

void sig_handler(int sig){
    switch (sig) {
        case SIGTSTP:
        case SIGINT:
            //salida por señal al sistema
            printf("\n");
            command_manager_exit();
            exit(0);

    }
}

int main(){
    //init
    signal(SIGINT,sig_handler);
    signal(SIGTSTP, sig_handler);
    command_manager_init();
    int interpreter_code = NORMAL;
    char input_buffer[INPUT_BUFFER_SIZE] = {0};
    char * tokens[TOKEN_BUFFER_SIZE] = {NULL};
    int token_number;

    struct termios old_tio, new_tio;
    unsigned char c;

    /* get the terminal settings for stdin */
    tcgetattr(0,&old_tio);

    /* we want to keep the old setting to restore them a the end */
    new_tio=old_tio;

    /* disable canonical mode (buffered i/o) and local echo */
    new_tio.c_lflag &=(~ICANON & ~ECHO);

    /* set the new settings immediately */
    tcsetattr(0,TCSANOW,&new_tio);

    do {
        c=getchar();
        printf("%c",c);
    } while(c!='q');

    /* restore the former settings */
    tcsetattr(0,TCSANOW,&old_tio);

    //sección repetida
    while(interpreter_code != EXIT){
        print_prompt();

        if(!read_input(input_buffer)){
            fprintf(stderr, "ERROR EN LA LECTURA DE DATOS: ENTRADA DEMASIADO GRANDE\n");
            continue;
        }


        history_append(input_buffer);

        if((token_number = tokenize(tokens, input_buffer)) == -1){
            fprintf(stderr, "ERROR EN LA CONVERSION A TOKENS: DEMASIADOS TOKENS\n");
            continue;
        }else if(!token_number) {
            history_pop();
            continue;
        }

        command_entry comm = get_command(tokens[0]);

        interpreter_code = comm.code;

        if(interpreter_code == NOTFOUND){
            fprintf(stderr, "ERROR AL BUSCAR COMANDO: NO ENCONTRADO\n");
            continue;
        }



        comm.command(&tokens[1], token_number - 1);
    }
    //fin
    command_manager_exit();
    return 0;
}