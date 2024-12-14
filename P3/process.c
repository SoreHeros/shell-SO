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
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

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


void showvar(char **, int){
	for(int i = 0; arg3[i] != NULL; i++){
		printf("%s\n", arg3[i]);
	}

	for(int i = 0; environ[i] != NULL; i++){
		printf("%s\n", environ[i]);
	}
	//getenv();
}
void showvar_help(){
    printf("\tshowvar [varName] [...]\n");
    printf("shows the content and direction of the enviroment variable\n");
	printf("empty:\tprints all enviroment variables\n");
    printf("varname:\tthe name of the variable to be printed\n");
}


void changevar(char **, int){

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


void subsvar(char **, int){

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


void environ_command(char **, int){

}
void environ_help(){
    printf("\tenviron [-environ|-addr]\n");
    printf("prints all the enviroment variables\n");
    printf("empty:\tuses arg3\n");
    printf("-environ:\tuses environ\n");
    printf("-addr:\tprints the address of arg3 and environ\n");
}


void fork_command(char **, int){

}
void fork_help(){
    printf("\tfork\n");
    printf("calls fork and waits for the child process to end\n");
}


void search(char **, int){

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


void exec(char **, int){

}
void exec_help(){
    printf("\texec [varName [...]] execName [arg [...]]\n");
    printf("executes a command on the same process\n");
    printf("varName:\tenviroment variable name (in environ)\n");
    printf("execName:\tname of the command to execute\n");
    printf("arg:\targuments to the commnad\n");
}


void execpri(char **, int){

}
void execpri_help(){
	printf("\texecpri prio [varName [...]] execName [arg [...]]\n");
    printf("executes a command on the same process with priority prio\n");
	printf("prio:\tpriority\n");
	printf("varName:\tenviroment variable name (in environ)\n");
	printf("execName:\tname of the command to execute\n");
	printf("arg:\targuments to the commnad\n");
}


void fg(char **, int){

}
void fg_help(){
	printf("\tfg [varName [...]] execName [arg [...]]\n");
	printf("executes a command on the same process\n");
	printf("varName:\tenviroment variable name (in environ)\n");
	printf("execName:\tname of the command to execute\n");
	printf("arg:\targuments to the commnad\n");
}


void fgpri(char **, int){

}
void fgpri_help(){
	printf("\tfgpri prio [varName [...]] execName [arg [...]]\n");
	printf("executes a command on the same process with priority prio\n");
	printf("prio:\tpriority\n");
	printf("varName:\tenviroment variable name (in environ)\n");
	printf("execName:\tname of the command to execute\n");
	printf("arg:\targuments to the commnad\n");
}


void back(char **, int){

}
void back_help(){
	printf("\tback [varName [...]] execName [arg [...]]\n");
	printf("executes a command on the same process\n");
	printf("varName:\tenviroment variable name (in environ)\n");
	printf("execName:\tname of the command to execute\n");
	printf("arg:\targuments to the commnad\n");
}


void backpri(char **, int){

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