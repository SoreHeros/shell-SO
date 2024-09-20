objects = $(patsubst %.c, %.o, $(wildcard p0/*.c) lists.c)

p0.out: p0.c $(objects)
	gcc -Wall -o p0.out p0.c *.o p0/*.o

run: p0.out
	./p0.out

%.o: %.c %.h
	gcc -Wall -c $< -o $@

.PHONY: clean

clean:
	rm *.out *.o p0/*.o
