//Heros Vicente González heros.vicente@udc.es

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <termio.h>

#include "P0/command_manager.h"

struct termios old_tio;

void sig_handler(int sig){
    switch (sig) {
        case SIGQUIT:
        case SIGTSTP:
        case SIGINT:
            //salida por señal al sistema
            printf("\n");
            command_manager_exit();
            tcsetattr(0,TCSANOW,&old_tio);
            exit(0);//todo cambiar de una salida directa a una variable???
    }
}

int main(){
    //init
    /* get the terminal settings for stdin */
    tcgetattr(0,&old_tio);//ajustes para devolver al parar la señal
    srand(time(NULL));
    signal(SIGINT,sig_handler);
    signal(SIGQUIT, sig_handler);
    signal(SIGTSTP, sig_handler);
    command_manager_init();
    int interpreter_code = NORMAL;
    char input_buffer[INPUT_BUFFER_SIZE] = {0};
    char * tokens[TOKEN_BUFFER_SIZE] = {NULL};
    int token_number;

    //sección repetida
    while(interpreter_code != EXIT){
        print_prompt(PROMPT);

        if(!read_input(input_buffer)){
            fprintf(stderr, "ERROR EN LA LECTURA DE DATOS: ENTRADA DEMASIADO GRANDE\n");
            continue;
        }

        if(strlen(input_buffer) == 0)//ya saltarselo si está vacío
            continue;



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