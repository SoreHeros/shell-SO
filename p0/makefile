.PHONY: all

all: sys_proc_info.o basic_functions.o files.o

basoc_functions.o: basic_functions.c basic_functions.h
	gcc -c basic_functions.c

sys_proc_info.o: sys_proc_info.c sys_proc_info.h
	gcc -c sys_proc_info.c

files.o: files.c files.h
	gcc -c files.c