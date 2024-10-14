//Heros Vicente González heros.vicente@udc.es

#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>

#include "p0/command_manager.h"
#include "p0/lists.h"

#define INPUT_BUFFER_SIZE 1024
#define PROMPT "terminal:"
#define ARRIBA 0x1B5B41
#define ABAJO 0x1B5B42
#define DERECHA 0x1B5B43
#define IZQUIERDA 0x1B5B44
#define SUPR 0x1B5B33
#define BACKSPACE 127

void get_cursor_pos(int * x, int * y){
    printf("\33[6n");
    scanf("\33[%i;%iR", y, x);
    (*x)--;
}
void set_cursor_pos(int x, int y){
    printf("\33[%i;%iH", y, x + 1);
}
void clean_from_cursor(){
    printf("\33[0J");
}
void buffer_insert(char * string, int pos, char insert){
    int len = strlen(string) + 1;
    if(len >= INPUT_BUFFER_SIZE)
        len = INPUT_BUFFER_SIZE;
    for(int i = len; i > pos; i--)
        string[i] = string[i-1];
    string[pos] = insert;
    string[INPUT_BUFFER_SIZE - 1] = '\0';
}
void buffer_delete(char * string, int pos){
    int len = strlen(string);
    for(int i = pos + 1; i <= len; i++)
        string[i-1] = string[i];
}

//lee la entrada y la procesa en tokens, devuelve 1, si la entrada es demasiado grande devuelve 0
int read_input(char * string){

    int string_pos, focused_command, ox, oy;
    struct termios old_tio, new_tio;

    /* get the terminal settings for stdin */
    tcgetattr(0,&old_tio);

    /* we want to keep the old setting to restore them a the end */
    new_tio=old_tio;

    /* disable canonical mode (buffered i/o) and local echo */
    new_tio.c_lflag &= ~ICANON & ~ECHO;

    /* set the new settings immediately */
    tcsetattr(0,TCSANOW,&new_tio);

    //inicializar
    string_pos = 0;
    string[0] = '\0';
    focused_command = history_len();
    get_cursor_pos(&ox, &oy);
    struct winsize ws;
    int c;
    do{
        //set cursor to cursor position
        ioctl(0, TIOCGWINSZ, &ws);//get window size
        set_cursor_pos((ox + string_pos) % ws.ws_col, oy + (ox + string_pos) / ws.ws_col);
        clean_from_cursor();
        printf("%s", &string[string_pos]);//asegurarse de que ocurra el wrap
        if(oy + (ox + strlen(string) - 1) / ws.ws_col >= ws.ws_row)//detectar si ha habido wrapping
            oy = ws.ws_row - (ox + strlen(string) - 1) / ws.ws_col;
        set_cursor_pos((ox + string_pos) % ws.ws_col, oy + (ox + string_pos) / ws.ws_col);//poner el cursor en el lugar correcto para el usuario
        c = getchar();
        if(c == '\33'){
            c <<= 8;
            c |= getchar();
            c <<= 8;
            c |= getchar();
            switch (c){
            case DERECHA:
                if(string_pos == strlen(string))
                    putchar('\7');//si no se puede ir a la derecha sonar campana
                else
                    string_pos++;
                break;
            case IZQUIERDA:
                if(string_pos == 0)
                    putchar('\7');//si no se puede ir a la izquierda sonar campana
                else
                    string_pos--;
                break;
            case SUPR:
                getchar();//recoger el caracter extra
                if(string_pos == strlen(string))
                    putchar('\7');//si no se puede borrar sonar campana
                else
                    buffer_delete(string, string_pos);
                break;
            case ARRIBA:
                if(focused_command == 0)
                    putchar('\7');//si no se puede ir más arriba sonar campana
                else{
                    if(focused_command == history_len())
                        history_append(string);//guardarlo solo si no se ha guardado ya
                    focused_command--;
                    strcpy(string, history_get(focused_command));//get history
                    set_cursor_pos(ox, oy);//poner cursor al principio
                    printf("%s", string);//imprimir string
                    string_pos = strlen(string);//marcar como leído
                }
                break;
            case ABAJO:
                if(focused_command == history_len())
                    putchar('\7');//si no se puede ir más abajo sonar campana
                else{
                    focused_command++;
                    strcpy(string, history_get(focused_command));//get history
                    if(focused_command == history_len() - 1)
                        history_pop(string);//sacarlo de la lista de histórico para evitar duplicados
                    set_cursor_pos(ox, oy);//poner cursor al principio
                    printf("%s", string);//imprimir string
                    string_pos = strlen(string);//marcar como leído
                }
                break;
            }
        }else{
            if(c == BACKSPACE){
                if(string_pos == 0)
                    putchar('\7');//si no se puede borrar sonar campana
                else{
                    string_pos--;
                    buffer_delete(string, string_pos);
                }
            }else if(c != '\n'){
                putchar(c);
                buffer_insert(string, string_pos, c);
                string_pos++;
                if(string_pos > INPUT_BUFFER_SIZE - 1){
                    putchar('\7');
                    string_pos = INPUT_BUFFER_SIZE - 1;
                }
            }
        }
    }while(c != '\n');

    /* restore the former settings */
    tcsetattr(0,TCSANOW,&old_tio);


    if(focused_command < history_len())//eliminar el string de historic si es necesario
        history_pop();

    putchar('\n');
    return 1;//como ya se maneja el tamaño máximo siempre va a retornar 1
}

void sig_handler(int sig){
    switch (sig) {
        case SIGTSTP:
        case SIGINT:
            //salida por señal al sistema
            printf("\n");
            command_manager_exit();
            exit(0);//todo cambiar de una salida directa a una variable

    }
}

int main(){
    //init
    srand(time(NULL));
    signal(SIGINT,sig_handler);
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

        if(strlen(input_buffer) <= 1)//ya saltarselo si está vacío
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