p0.out: p0.c p0 lists.o
	gcc -Wall -o p0.out p0.c *.o p0/*.o

run: p0.out
	./p0.out

lists.o: lists.c lists.h
	gcc -Wall -c lists.c

p0:
	cd p0 && make

.PHONY: clean p0

clean:
	rm *.out *.o p0/*.o