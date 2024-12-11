//
// Created by heros on 11/12/24.
//

#ifndef PROCESS_H
#define PROCESS_H

void getuid(char **, int);
void getuid_help();
void setuid(char **, int);
void setuid_help();
void showvar(char **, int);
void showvar_help();
void subsvar(char **, int);
void subsvar_help();
void environ(char **, int);
void environ_help();
void fork(char **, int);
void fork_help();
void search(char **, int);
void search_help();
void exec(char **, int);
void exec_help();
void execpri(char **, int);
void execpri_help();
void fg(char **, int);
void fg_help();
void fgpri(char **, int);
void fgpri_help();
void back(char **, int);
void back_help();
void backpri(char **, int);
void backpri_help();
void listjobs(char **, int);
void listjobs_help();
void deljobs(char **, int);
void deljobs_help();

#endif //PROCESS_H
