result = p0.out
main = p0.c
objects = $(patsubst %.c, %.o, $(wildcard p0/*.c))

$(result): $(main) $(objects)
	gcc -Wall -o $(result) $(main) $(objects)

#compilar todos los objetos
%.o: %.c %.h
	gcc -Wall -c $< -o $@

.PHONY: clean run leak

run: $(result)
	./$(result)

leak: $(result)
	valgrind --leak-check=full ./$(result)

clean:
	rm $(result) $(objects)
