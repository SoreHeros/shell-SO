//
// Created by heros on 24/09/24.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "command_manager.h"

#include <time.h>

#include "basic_functions.h"
#include "sys_proc_info.h"
#include "files.h"
#include "../P1/dirs.h"
#include "../P2/memory.h"
#include "../utils/lists.h"
#include "../utils/shared_vars.h"

void help(char **, int);
void help_help();

void historic(char **, int);
void historic_help();

void not_found(char **, int);
void not_found_help();

command_entry nf =     {"not found", NOTFOUND, not_found, not_found_help};

command_entry commands[] =
        {
                     {"quit",      EXIT,          quit,     quit_help},
                     {"exit",      EXIT,          exit_command,      exit_help},
                     {"bye",       EXIT,          bye,      bye_help},
                     {"help",      NORMAL,        help,     help_help},
                     {"authors",   NORMAL,        authors,  authors_help},
                     {"echo",      NORMAL,        echo,     echo_help},
                     {"pid",       NORMAL,        pid,      pid_help},
                     {"ppid",      NORMAL,        ppid,     ppid_help},
                     {"infosys",   NORMAL,        infosys,  infosys_help},
                     {"historic",  NORMAL,        historic, historic_help},
                    { "open",      NORMAL,        open_command,      open_help},
                    { "close",     NORMAL,        close_command,     close_help},
                    { "date",      NORMAL,        date,     date_help},
                    { "cd",        NORMAL,        cd,       cd_help},
                    { "dup",       NORMAL,        dup_command,       dup_help},
                    { "signals",   NORMAL,        signals,  signals_help},
                    { "makefile",  NORMAL,        makefile, makefile_help},
                    { "makedir",   NORMAL,        makedir,  makedir_help},
                    { "cwd",       NORMAL,        cwd,      cwd_help},
                    { "listfile",  NORMAL,        listfile, listfile_help},
                    { "listdir",   NORMAL,        listdir,  listdir_help},
                    //{ "reclist",   NORMAL,        reclist,  reclist_help},
                    { "allocate",  NORMAL,        allocate, allocate_help},
                    { "memdump",   NORMAL,        memdump,  memdump_help},
                    { "readfile",  NORMAL,        readfile, readfile_help},
                    { "memfill",   NORMAL,        memfill,  memfill_help},
                    { "stack",     NORMAL,        stack,    stack_help},
                    { "pmap",      NORMAL,        pmap_command,     pmap_help},
                    { "print",     NORMAL,        print,    print_help},
        };
command_entry ** commands_pointer;
int commands_len = sizeof(commands) / sizeof(command_entry);
list historial;
unsigned int static_seed;

//todo fix recursions
#define RECURSION_LIMIT 10

void historic_execute(int n){
    static int recursions = 0;

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
    printf("executing command %i: %s\n", n, str);
    char * tokens[TOKEN_BUFFER_SIZE];
    int token_number = tokenize(tokens, str);
    command_entry comd = get_command(tokens[0]);
    comd.command(&tokens[1], token_number - 1);
    free(str);
    recursions = 0;
}

void print_prompt(char * prompt){
    rand_r(&static_seed);
    unsigned int seed = static_seed;
    char code[8] = "\033[33m";
    for(int i = 0; prompt[i] != '\0'; i++){
        int r = rand_r(&seed);
        if(r&0b1)//brillante o no brillante
            code[2] = '3';
        else
            code[2] = '9';

        code[3] = '1' + r % 7; //todos los colores excepto el negro
        printf("%s%c", code, prompt[i]);//parseo raro para que lea el dato como si fuera un string
    }

    printf("\033[0m");
}

void historic(char ** tokens, int token_number){

    int historic_start = 0;

    if(token_number > 0){
        if(tokens[0][0] == '-')
            historic_start = list_length(historial) - atoi(&tokens[0][1]);
        else{
            historic_execute(atoi(tokens[0]));
            return;
        }
    }

    for(int i = historic_start, len = list_length(historial); i < len; i++)
        printf("%3i: %s\n", i,(char *)list_get(historial, i));
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
        int found = 0;
        for (int i = 0; i < token_number; i++) {
            command_entry cmd = get_command(tokens[i]);
            if (cmd.code != NOTFOUND) {
                cmd.help();
                printf("\n");
                found = 1;
            }
        }
        if(!found)
            printf("no commands found\n");
    }
}
void help_help(){
    printf("\thelp cmd [...]\nempty:\tprints every command's help\ncmd:\tprints the comand's help\n");
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
    static_seed = time(NULL);
    commands_pointer = malloc(commands_len * sizeof(command_entry *));
    for(int i = 0; i < commands_len; i++)
        commands_pointer[i] = &commands[i];
    qsort(commands_pointer, commands_len, sizeof(command_entry *), command_comparator);
    shared_vars_init();
    mallocs_init();
}
void command_manager_exit(){
    for(int i = 0; i < list_length(historial); i++)
        free(list_get(historial, i));
    list_free(historial);
    free(commands_pointer);
    shared_vars_exit();
    mallocs_exit();
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
int history_len(){
    return list_length(historial);
}
char * history_get(int pos){
    return list_get(historial, pos);
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

#define ARRIBA 0x1B5B41
#define ABAJO 0x1B5B42
#define DERECHA 0x1B5B43
#define IZQUIERDA 0x1B5B44
#define SUPR 0x1B5B33
#define BACKSPACE 127

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
void buffer_append(char * string, char insert){
    string[strlen(string) + 1] = '\0';
    string[strlen(string)] = insert;
}

//leer manualmente el valor de vuelta
int manual_read(int * x, int * y){
    char number[16] = {'\0'};
    char in;
    while(getchar() != '\33');//limpiar hasta el \33
    if(getchar() != '[')//leer el [
        return 0;
    while((in = getchar()) != ';')//leer hasta ; e ir guardando los números
        if(in <= 57 && in >= 48)
            buffer_append(number, in);
        else
            return 0;
    *y = atoi(number);
    number[0] = '\0';
    while((in = getchar()) != 'R')//leer hasta R e ir guardando los números
        if(in <= 57&& in >= 48)
            buffer_append(number, in);
        else
            return 0;
    *x = atoi(number);
    return 1;
}

void get_cursor_pos(int * x, int * y){//esta función hace que algunas veces se coma una letra, pero creo que es un sacrificio aceptable
    do{
        printf("\33[6n");
    }while(manual_read(x, y) == 0);
    (*x)--;
}

//lee la entrada y la procesa en tokens, devuelve 1, si la entrada es demasiado grande devuelve 0
int read_input(char * string){

    int focused_command, ox, oy;
    unsigned int string_pos;
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
    ox = strlen(PROMPT);
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
                        history_pop();//sacarlo de la lista de histórico para evitar duplicados
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