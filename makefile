result = p0.out
main = p0.c
objects = $(patsubst %.c, %.o, lists.c $(wildcard p0/*.c))

$(result): $(main) $(objects)
	gcc -Wall -o $(result) $(main) $(objects)

#compilar todos los objetos
%.o: %.c %.h
	gcc -Wall -c $< -o $@

.PHONY: clean run

run: $(result)
	./$(result)

clean:
	rm $(result) $(objects)
