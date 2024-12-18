//
// Created by heros on 11/12/24.
//

#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "../P2/memory.h"
#include "../utils/shared_vars.h"

/*el siguiente codigo se da como ayuda por si se quiere utilizar*/
/*NO ES OBLIGATORIO USARLO*/
/*y pueden usarse funciones enteras o parte de funciones */

/*Este fichero, ayudaP3.c no estÃ¡ pensado para ser compilado separadamente */
/*, entre otras cosas, no contiene los includes necesarios*/
/*y las constantes utilizadas, no estÃ¡n definidas en Ã©l*/

//void Cmd_fork (char *tr[])
//{
//	pid_t pid;
//
//	if ((pid=fork())==0){
///*		VaciarListaProcesos(&LP); Depende de la implementaciÃ³n de cada uno*/
//		printf ("ejecutando proceso %d\n", getpid());
//	}
//	else if (pid!=-1)
//		waitpid (pid,NULL,0);
//}
//
//int BuscarVariable (char * var, char *e[])  /*busca una variable en el entorno que se le pasa como parÃ¡metro*/
//{                                           /*devuelve la posicion de la variable en el entorno, -1 si no existe*/
//  int pos=0;
//  char aux[MAXVAR];
//
//  strcpy (aux,var);
//  strcat (aux,"=");
//
//  while (e[pos]!=NULL)
//    if (!strncmp(e[pos],aux,strlen(aux)))
//      return (pos);
//    else
//      pos++;
//  errno=ENOENT;   /*no hay tal variable*/
//  return(-1);
//}
//
//
//int CambiarVariable(char * var, char * valor, char *e[]) /*cambia una variable en el entorno que se le pasa como parÃ¡metro*/
//{                                                        /*lo hace directamente, no usa putenv*/
//  int pos;
//  char *aux;
//
//  if ((pos=BuscarVariable(var,e))==-1)
//    return(-1);
//
//  if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
//	return -1;
//  strcpy(aux,var);
//  strcat(aux,"=");
//  strcat(aux,valor);
//  e[pos]=aux;
//  return (pos);
//}
//
//char * Ejecutable (char *s)
//{
//        static char path[MAXNAME];
//        struct stat st;
//        char *p;
//
//        if (s==NULL || (p=SearchListFirst())==NULL)
//                return s;
//        if (s[0]=='/' || !strncmp (s,"./",2) || !strncmp (s,"../",3))
//        return s;        /*is an absolute pathname*/
//
//        strncpy (path, p, MAXNAME-1);strncat (path,"/",MAXNAME-1); strncat(path,s,MAXNAME-1);
//        if (lstat(path,&st)!=-1)
//                return path;
//        while ((p=SearchListNext())!=NULL){
//            strncpy (path, p, MAXNAME-1);strncat (path,"/",MAXNAME-1); strncat(path,s,MAXNAME-1);
//            if (lstat(path,&st)!=-1)
//                   return path;
//        }
//        return s;
//}
//
//int Execpve(char *tr[], char **NewEnv, int * pprio)
//{
//    char *p;               /*NewEnv contains the address of the new environment*/
//                           /*pprio the address of the new priority*/
//                           /*NULL indicates no change in environment and/or priority*/
//    if (tr[0]==NULL || (p=Ejecutable(tr[0]))==NULL){
//        errno=EFAULT;
//        return-1;
//        }
//    if (pprio !=NULL  && setpriority(PRIO_PROCESS,getpid(),*pprio)==-1 && errno){
//                        printf ("Imposible cambiar prioridad: %s\n",strerror(errno));
//                        return -1;
//        }
//
//        if (NewEnv==NULL)
//                return execv(p,tr);
//        else
//                return execve (p, tr, NewEnv);
//}
//
///*las siguientes funciones nos permiten obtener el nombre de una senal a partir
//del nÃºmero y viceversa */
//static struct SEN sigstrnum[]={
//	{"HUP", SIGHUP},
//	{"INT", SIGINT},
//	{"QUIT", SIGQUIT},
//	{"ILL", SIGILL},
//	{"TRAP", SIGTRAP},
//	{"ABRT", SIGABRT},
//	{"IOT", SIGIOT},
//	{"BUS", SIGBUS},
//	{"FPE", SIGFPE},
//	{"KILL", SIGKILL},
//	{"USR1", SIGUSR1},
//	{"SEGV", SIGSEGV},
//	{"USR2", SIGUSR2},
//	{"PIPE", SIGPIPE},
//	{"ALRM", SIGALRM},
//	{"TERM", SIGTERM},
//	{"CHLD", SIGCHLD},
//	{"CONT", SIGCONT},
//	{"STOP", SIGSTOP},
//	{"TSTP", SIGTSTP},
//	{"TTIN", SIGTTIN},
//	{"TTOU", SIGTTOU},
//	{"URG", SIGURG},
//	{"XCPU", SIGXCPU},
//	{"XFSZ", SIGXFSZ},
//	{"VTALRM", SIGVTALRM},
//	{"PROF", SIGPROF},
//	{"WINCH", SIGWINCH},
//	{"IO", SIGIO},
//	{"SYS", SIGSYS},
///*senales que no hay en todas partes*/
//#ifdef SIGPOLL
//	{"POLL", SIGPOLL},
//#endif
//#ifdef SIGPWR
//	{"PWR", SIGPWR},
//#endif
//#ifdef SIGEMT
//	{"EMT", SIGEMT},
//#endif
//#ifdef SIGINFO
//	{"INFO", SIGINFO},
//#endif
//#ifdef SIGSTKFLT
//	{"STKFLT", SIGSTKFLT},
//#endif
//#ifdef SIGCLD
//	{"CLD", SIGCLD},
//#endif
//#ifdef SIGLOST
//	{"LOST", SIGLOST},
//#endif
//#ifdef SIGCANCEL
//	{"CANCEL", SIGCANCEL},
//#endif
//#ifdef SIGTHAW
//	{"THAW", SIGTHAW},
//#endif
//#ifdef SIGFREEZE
//	{"FREEZE", SIGFREEZE},
//#endif
//#ifdef SIGLWP
//	{"LWP", SIGLWP},
//#endif
//#ifdef SIGWAITING
//	{"WAITING", SIGWAITING},
//#endif
// 	{NULL,-1},
//	};
//
//
//int ValorSenal(char * sen)  /*devuelve el numero de senial a partir del nombre*/
//{
//  int i;
//  for (i=0; sigstrnum[i].nombre!=NULL; i++)
//  	if (!strcmp(sen, sigstrnum[i].nombre))
//		return sigstrnum[i].senal;
//  return -1;
//}
//
//
//char *NombreSenal(int sen)  /*devuelve el nombre senal a partir de la senal*/
//{			/* para sitios donde no hay sig2str*/
// int i;
//  for (i=0; sigstrnum[i].nombre!=NULL; i++)
//  	if (sen==sigstrnum[i].senal)
//		return sigstrnum[i].nombre;
// return ("SIGUNKNOWN");
//}
extern char ** environ;


void getuid_command(char **, int){
	struct passwd * user = getpwuid(getuid());
	printf("Real credential:\t %5i, (%s)\n", user->pw_uid, user->pw_name);
	user = getpwuid(geteuid());
	printf("Effective credential:\t %5i, (%s)\n", user->pw_uid, user->pw_name);
}
void getuid_help(){
    printf("\tgetuid\n");
    printf("prints the process's credentials\n");
}


void setuid_command(char ** tokens, int token_number){
	if(token_number < 1){
		fprintf(stderr, "ERROR ON PARSING: function must have at least 1 argument");
		return;
	}

	uid_t uid;

	if(!strcmp(tokens[0], "-l")){
		if(token_number < 2){
			fprintf(stderr, "ERROR ON PARSING: -l must have at least 1 other argument");
			return;
		}
		struct passwd * pw = getpwnam(tokens[1]);

		if(pw == NULL){
			perror("ERROR ON GETTING UID FROM LOGIN");
			return;
		}

		uid = pw->pw_uid;

	}else{
		uid  = atoi(tokens[0]);
	}

	struct passwd * pw = getpwuid(uid);

	if(pw == NULL){
		perror("ERROR ON READING PW FILE");
		return;
	}

	if(seteuid(uid))
		perror("ERROR ON SETTING NEW UID");
	else
		printf("Effective credentials set to %i (%s)\n", uid, pw->pw_name);
}
void setuid_help(){
    printf("\tsetuid [-l] id\n");
    printf("sets the process's effective credentials\n");
    printf("-l:\tfor loggin\n");
    printf("id:\tdesired urser's id\n");
}


void showvar(char ** tokens, int token_number){
	list enviroment = get_enviroment();
	list pmap = get_pmap();

	printf("idx     arg3     addr, idx    environ   addr,       getenv addr,                          name, value\n");

	if(token_number == 0)
	{
		for(int i = 0; i < list_length(enviroment); i++){
			envVar * v = list_get(enviroment, i);
			printf("%3i ", v->arg3);
			print_colored_pointer(pmap, v->arg3S);
			printf(" %3i ", v->environ);
			print_colored_pointer(pmap, v->environS);
			printf(" ");
			char * str;
			print_colored_pointer(pmap, str = getenv(v->name));
			printf(" %30s", v->name);

			if((v->arg3S != NULL && v->environS != NULL && strcmp(v->arg3S, v->environS)) || (v->environS != NULL && str != NULL && strcmp(v->environS, str)) || (v->arg3S != NULL && str != NULL && strcmp(v->arg3S, str))){
				printf(" \33[31mWARNING\33[0m: some of the pointes don't point to the same content, printing all\n");
				printf("                                                                                                %s\n", v->arg3S);
				printf("                                                                                                %s\n", v->environS);
				printf("                                                                                                %s\n", str);
			}else{
				if(v->arg3S != NULL)
					printf("\t%s\n", v->arg3S);
				else if(v->	environS != NULL)
					printf("\t%s\n", v->environS);
				else
					printf("\t%s\n", str);
			}
		}
	}else{
		for(int i = 0; i < token_number; i++){
			int index = list_search(enviroment, tokens[i], envVarNameComp);
			if(index < 0){
				fprintf(stderr, "ERROR ON SEARCH: %s doesen't exist\n", tokens[i]);
			}else{
				envVar * v = list_get(enviroment, index);
				printf("%3i ", v->arg3);
				print_colored_pointer(pmap, v->arg3S);
				printf(" %3i ", v->environ);
				print_colored_pointer(pmap, v->environS);
				printf(" ");
				char * str;
				print_colored_pointer(pmap, str = getenv(v->name));
				printf(" %30s", v->name);

				if((v->arg3S != NULL && v->environS != NULL && strcmp(v->arg3S, v->environS)) || (v->environS != NULL && str != NULL && strcmp(v->environS, str)) || (v->arg3S != NULL && str != NULL && strcmp(v->arg3S, str))){
					printf(" \33[31mWARNING\33[0m: some of the pointes don't point to the same content, printing all\n");
					printf("                                                                                                %s\n", v->arg3S);
					printf("                                                                                                %s\n", v->environS);
					printf("                                                                                                %s\n", str);
				}else{
					if(v->arg3S != NULL)
						printf("\t%s\n", v->arg3S);
					else if(v->	environS != NULL)
						printf("\t%s\n", v->environS);
					else
						printf("\t%s\n", str);
				}
			}
		}
	}
}
void showvar_help(){
    printf("\tshowvar [varName] [...]\n");
    printf("shows the content and direction of the enviroment variable\n");
	printf("empty:\tprints all enviroment variables\n");
    printf("varname:\tthe name of the variable to be printed\n");
}


void changevar(char ** tokens, int token_number){
	if(token_number < 3){
		fprintf(stderr, "ERROR ON PARSING: command must have 3 arguments\n");
		return;
	}

	char type = '\0';

	if(tokens[0][0] == '-' && tokens[0][2] == '\0')
		type = tokens[0][1];
	else{
		fprintf(stderr, "ERROR ON PARSING: %s isnt a valid type\n", tokens[0]);
		return;
	}

	list envVars = get_enviroment();

	int index = list_search(envVars, tokens[1], envVarNameComp);

	if(index < 0 && (type == 'a' || type == 'e')){
		fprintf(stderr, "ERROR ON PARSING: var %s doesen't exist\n", tokens[1]);
		return;
	}

	if(type == 'a'){
		envVar * v = list_get(envVars, index);
		if(v->arg3 < 0){
			fprintf(stderr, "ERROR ON PARSING: var %s doesen't exist on arg3\n", tokens[1]);
			return;
		}
		char * p = arg3[v->arg3];

		list pmap = get_pmap();
		page * pag = get_pointer_page(pmap, p);

		if(!(pag->perms & PAGE_STACK))
			free(p);
		asprintf(&arg3[v->arg3], "%s=%s", tokens[1], tokens[2]);
	}else if(type == 'e'){
		envVar * v = list_get(envVars, index);
		if(v->environ < 0){
			fprintf(stderr, "ERROR ON PARSING: var %s doesen't exist on environ\n", tokens[1]);
			return;
		}

		char * p = environ[v->environ];

		list pmap = get_pmap();
		page * pag = get_pointer_page(pmap, p);

		if(!(pag->perms & PAGE_STACK))
			free(p);
		asprintf(&environ[v->environ], "%s=%s", tokens[1], tokens[2]);
	}else if(type == 'p'){
		char * str;
		asprintf(&str, "%s=%s", tokens[1], tokens[2]);
		int retval = putenv(str);
		if(retval){
			perror("ERROR ON PUTENV");
		}
	}else{
		fprintf(stderr, "ERROR ON PARSING: %s isnt a valid type\n", tokens[0]);
	}
}
void changevar_help(){
	printf("\tchangevar -a|-e|-p varName value\n");
    printf("replaces the value of varName with value\n");
	printf("-a:\tuses arg3\n");
	printf("-e:\tuses environ\n");
	printf("-p:\tuses putenv (can create new variables)\n");
	printf("varname:\tname of en enviroment variable\n");
	printf("value:\tnew value of the enviroment variable\n");
}


void subsvar(char ** tokens, int token_number)
{
	if(token_number < 3){
		fprintf(stderr, "ERROR ON PARSING: function needs at least 3 arguments\n");
		return;
	}

	int isArg3 = 0;

	if(!strcmp(tokens[0], "-a") || !strcmp(tokens[0], "-e")){
		isArg3 = tokens[0][1] == 'a';
	}else{
		fprintf(stderr, "ERROR ON PARSING: %s isn't a valid mode\n", tokens[0]);
		return;
	}

	list envVars = get_enviroment();

	int index = list_search(envVars, tokens[1], envVarNameComp);

	if(index < 0){
		fprintf(stderr, "ERROR ON PARSING: variable %s doesn't exist\n", tokens[1]);
		return;
	}

	envVar * v = list_get(envVars, index);

	if(isArg3 && v->arg3 < 0){
		fprintf(stderr, "ERROR ON PARSING: variable %s doesn't exist on arg3\n", tokens[1]);
		return;
	}

	if(!isArg3 && v->environ < 0){
		fprintf(stderr, "ERROR ON PARSING: variable %s doesn't exist on environ\n", tokens[1]);
		return;
	}

	int resIndex = list_search(envVars, tokens[2], envVarNameComp);

	if(resIndex >= 0)
	{
		envVar * r = list_get(envVars, resIndex);

		if(isArg3 && r->arg3 >= 0){
			fprintf(stderr, "ERROR ON PARSING: variable %s already exists on arg3[%i]\n", tokens[2], r->arg3);
			return;
		}

		if(!isArg3 && r->environ >= 0){
			fprintf(stderr, "ERROR ON PARSING: variable %s already exists on environ[%i]\n", tokens[2], r->environ);
			return;
		}
	}

	char ** slot;
	char * str;

	if(isArg3){
		slot = &arg3[v->arg3];
		if(token_number < 4)
			str = v->arg3S;
		else
			str = tokens[3];
	}else{
		slot = &environ[v->environ];
		if(token_number < 4)
			str = v->environS;
		else
			str = tokens[3];
	}

	char * newstr;

	asprintf(&newstr, "%s=%s", tokens[2], str);

	list pmap = get_pmap();

	page * p = get_pointer_page(pmap, *slot);

	if(!(p->perms & PAGE_STACK)){
		free(*slot);
	}
	*slot = newstr;
}
void subsvar_help(){
    printf("\tsubsvar -a|-e oldVarName newVarName [newValue]\n");
    printf("replaces the name of oldVarName with newVarName and the value with newValue (if not provided it keeps the same value)\n");
    printf("-a:\tuses arg3\n");
    printf("-e:\tuses environ\n");
    printf("oldvarname:\tname of the variable to substitute\n");
    printf("newvarname:\tnew name of the variable to substitute\n");
    printf("newvalue:\tnew value to asign the variable\n");
}


void environ_command(char ** tokens, int token_number){
	list pmap = get_pmap();
	if(token_number){
		if(!strcmp(tokens[0], "-environ")){
			for(int i = 0; environ[i] != NULL; i++){
				printf("%3i ", i);
				print_colored_pointer(pmap, environ[i]);
				printf(" %s\n", environ[i]);
			}
		}else if(!strcmp(tokens[0], "-addr")){
			printf("arg3:\t");
			print_colored_pointer(pmap, arg3);
			printf("\nenviron:");
			print_colored_pointer(pmap, environ);
			printf("\n");
		}else{
			fprintf(stderr, "ERROR ON PARSING: %s isn't a valid flag\n", tokens[0]);
		}
		return;
	}
	for(int i = 0; arg3[i] != NULL; i++){
		printf("%3i ", i);
		print_colored_pointer(pmap, arg3[i]);
		printf(" %s\n", arg3[i]);
	}
}
void environ_help(){
    printf("\tenviron [-environ|-addr]\n");
    printf("prints all the enviroment variables\n");
    printf("empty:\tuses arg3\n");
    printf("-environ:\tuses environ\n");
    printf("-addr:\tprints the address of arg3 and environ\n");
}

void fork_command(char **, int){
	pid_t son;
	if(!((son=fork()))){
		printf ("ejecutando proceso %d\n", getpid());//todo add to process list
	}else if(son != -1)
		waitpid(son, NULL, 0);
	else
		perror("ERROR ON FORKING");
}
void fork_help(){
    printf("\tfork\n");
    printf("calls fork and waits for the child process to end\n");
}



void search_command(char ** tokens, int token_number){
	if(token_number < 1){
		if(list_length(search)){
			for(int i = 0; i < list_length(search); i++)
				printf("%s\n", (char *)list_get(search, i));
		}else{
			printf("Search list is empty\n");
		}
		return;
	}

	if(!strcmp(tokens[0], "-add")){
		if(token_number < 2){
			fprintf(stderr, "ERROR ON PARSING: -add must have 2 arguments\n");
			return;
		}

		if(list_search(search, tokens[1], (int (*)(const void *,const void *))strcmp) != -1){
			fprintf(stderr, "ERROR ON ADDING: %s already exists\n", tokens[1]);
			return;
		}

		if(tokens[1][0] != '/'){
			fprintf(stderr, "ERROR ON ADDING: %s isn't an absolute path\n", tokens[1]);
			return;
		}

		list_append(search, strdup(tokens[1]));

	}else if(!strcmp(tokens[0], "-del")){
		if(token_number < 2){
			fprintf(stderr, "ERROR ON PARSING: -del must have 2 arguments\n");
			return;
		}

		int pos;

		if((pos = list_search(search, tokens[1], (int (*)(const void *,const void *))strcmp)) == -1){
			fprintf(stderr, "ERROR ON ADDING: %s doesen't exist\n", tokens[1]);
			return;
		}

		free(list_get(search, pos));
		list_remove(search, pos);

	}else if(!strcmp(tokens[0], "-clear")){
		while (list_length(search))
			free(list_pop(search));
	}else if(!strcmp(tokens[0], "-path")){
		int added = 0;

		char * path = strdup(getenv("PATH"));

		char * dirs[256] = {NULL};

		int len = 1;

		dirs[0] = strtok(path, ":");

		while (len < 256 && (dirs[len] = strtok(NULL, ":")) != NULL)
			len++;

		for(int i = 0; i < len; i++){
			if(list_search(search, dirs[i], (int (*)(const void *,const void *))strcmp) == -1){
				added++;
				list_append(search, strdup(dirs[i]));
			}
		}

		free(path);

		printf("Added to search %i elements from PATH\n", added);
	}else{
		fprintf(stderr, "ERROR ON PARSING: %s isn't a valid flag\n", tokens[0]);
	}
}
void search_help(){
    printf("\tsearch [-add dir|-del dir|-clear|-path]\n");
    printf("empty:\tprints the current search dirs\n");
    printf("dir:\tdirectory path\n");
    printf("-add:\tadds dir to the search list\n");
    printf("-del:\tdeletes dir from the search list\n");
    printf("-clear:\tclears the seach list\n");
    printf("-path:\tadds the dirs in PATH to the search list\n");
}

typedef struct{
	char * path;
	char * args[256];
	char * env[256];
}ExecInput;
ExecInput processExecInput(char ** tokens, int token_number){
	ExecInput ret = {NULL};
	int count = 0;
	while(tokens[count] != NULL && getenv(tokens[count]) != NULL)
		count++;

	for(int i = 0; i < count; i++)
		ret.env[i] = tokens[i];

	if(!access(tokens[count], F_OK))
		ret.path = tokens[count];
	else for(int i = 0; i < list_length(search); i++){
		static char * path = NULL;
		asprintf(&path, "%s/%s", (char *) list_get(search, i), tokens[count]);
		if(!access(path, F_OK)){
			ret.path = path;
			break;
		}
	}

	ret.args[0] = ret.path;

	for(int i = count + 1, j = 1; i < token_number; i++, j++)
		ret.args[j] = tokens[i];

	return ret;
}


void exec(char ** tokens, int token_number){

	if(token_number < 1){
		fprintf(stderr, "ERROR ON PARSING: must have at least 1 argument\n");
		return;
	}

	ExecInput input = processExecInput(tokens, token_number);

	if(input.env[0] != NULL){
		if(execve(input.path, input.args, input.env))
			perror("ERROR ON EXECUTE");
	}else{
		if(execv(input.path, input.args))
			perror("ERROR ON EXECUTE");
	}
}
void exec_help(){
    printf("\texec [varName [...]] execName [arg [...]]\n");
    printf("executes a command on the same process\n");
    printf("varName:\tenviroment variable name (in environ)\n");
    printf("execName:\tname of the command to execute\n");
    printf("arg:\targuments to the commnad\n");
}


void execpri(char ** tokens, int token_number){
	if(token_number < 2){
		fprintf(stderr, "ERROR ON PARSING: must have at least 2 argument\n");
		return;
	}

    if(setpriority(PRIO_PROCESS,getpid(),atoi(tokens[0]))==-1 && errno){
    	perror("ERROR ON SETTING PRIORITY");
    	return;
    }

	exec(&tokens[1], token_number - 1);
}
void execpri_help(){
	printf("\texecpri prio [varName [...]] execName [arg [...]]\n");
    printf("executes a command on the same process with priority prio\n");
	printf("prio:\tpriority\n");
	printf("varName:\tenviroment variable name (in environ)\n");
	printf("execName:\tname of the command to execute\n");
	printf("arg:\targuments to the commnad\n");
}


void fg(char ** tokens, int token_number){
	pid_t son;
	if(!((son=fork()))){
		exec(tokens, token_number);//todo add to process list
		exit(-1);
	}else if(son != -1)
		waitpid(son, NULL, 0);
	else
		perror("ERROR ON FORKING");
}
void fg_help(){
	printf("\tfg [varName [...]] execName [arg [...]]\n");
	printf("executes a command on the same process\n");
	printf("varName:\tenviroment variable name (in environ)\n");
	printf("execName:\tname of the command to execute\n");
	printf("arg:\targuments to the commnad\n");
}


void fgpri(char ** tokens, int token_number){
	pid_t son;
	if(!((son=fork()))){
		execpri(tokens, token_number);//todo add to process list
		exit(-1);
	}else if(son != -1)
		waitpid(son, NULL, 0);
	else
		perror("ERROR ON FORKING");
}
void fgpri_help(){
	printf("\tfgpri prio [varName [...]] execName [arg [...]]\n");
	printf("executes a command on the same process with priority prio\n");
	printf("prio:\tpriority\n");
	printf("varName:\tenviroment variable name (in environ)\n");
	printf("execName:\tname of the command to execute\n");
	printf("arg:\targuments to the commnad\n");
}


void back(char ** tokens, int token_number){
	pid_t son;
	if(!((son=fork()))){
		exec(tokens, token_number);//todo add to process list
		exit(-1);
	}else if(son != -1)
		return;
	else
		perror("ERROR ON FORKING");
}
void back_help(){
	printf("\tback [varName [...]] execName [arg [...]]\n");
	printf("executes a command on the same process\n");
	printf("varName:\tenviroment variable name (in environ)\n");
	printf("execName:\tname of the command to execute\n");
	printf("arg:\targuments to the commnad\n");
}


void backpri(char ** tokens, int token_number){
	pid_t son;
	if(!((son=fork()))){
		execpri(tokens, token_number);//todo add to process list
		exit(-1);
	}else if(son != -1)
		return;
	else
		perror("ERROR ON FORKING");
}
void backpri_help(){
	printf("\tbackpri prio [varName [...]] execName [arg [...]]\n");
	printf("executes a command on the same process with priority prio\n");
	printf("prio:\tpriority\n");
	printf("varName:\tenviroment variable name (in environ)\n");
	printf("execName:\tname of the command to execute\n");
	printf("arg:\targuments to the commnad\n");
}


void listjobs(char **, int){
	if(!list_length(jobs)){
		printf("Jobs list is empty");
		return;
	}

	for(int i = 0; i < list_length(jobs); i++){
		printf("%5i\n", *(int *)list_get(jobs, i));
	}
}
void listjobs_help(){
    printf("\tlistjobs\n");
    printf("list the background processes\n");
}


void deljobs(char **, int){

}
void deljobs_help(){
    printf("\tdeljobs -term|-sig\n");
    printf("-term:\tdeletes every terminated process\n");
    printf("-sig:\tdeletes every signaled process\n");
}

void generic_execute(char ** tokens, int token_number){
	fg(tokens, token_number);
}