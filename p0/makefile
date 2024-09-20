.PHONY: all

all: sys_proc_info.o basic_functions.o files.o

basic_functions.o: basic_functions.c basic_functions.h
	gcc -Wall -c basic_functions.c

sys_proc_info.o: sys_proc_info.c sys_proc_info.h
	gcc -Wall -c sys_proc_info.c

files.o: files.c files.h
	gcc -Wall -c files.c