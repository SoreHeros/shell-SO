#include <stdio.h>
#include <string.h>

#define INPUT_BUFFER_SIZE 1024

typedef const struct{
    char * name;
    void (*function)(char *, int);
}command;


void test_function(char * string, int len){
    fputs(string, stdout);
}

command commands[] =
        {"test", test_function};

//imprime el inicio
void print_prompt(){
    printf("temporal:");
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
            printf("ERROR EN LA LECTURA DE DATOS: ENTRADA DEMASIADO GRANDE\n");
            continue;
        }



        commands[0].function(input_buffer, 0);
        exit = 1;
    }

    //fin
    return 0;
}