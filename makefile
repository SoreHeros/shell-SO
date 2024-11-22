result = p2
main = p2.c
objects = $(patsubst %.c, %.o, $(wildcard P0/*.c) $(wildcard P1/*.c) $(wildcard P2/*.c) $(wildcard utils/*.c))
flags = -Wall -Wextra

$(result): $(main) $(objects)
	gcc $(flags) -o $(result) $(main) $(objects)

#compilar todos los objetos
%.o: %.c %.h
	gcc $(flags) -c -o $@ $<

comp:
	zip -r9 ../$(result).zip . -x ".*" "*.o" "*.out" "*.md" "*.pdf" "*.txt" "testing*" "cmake*" "$(result)"

.PHONY: clean run leak

run: $(result)
	./$(result)

leak: $(result)
	valgrind --leak-check=full --show-leak-kinds=all ./$(result)

clean:
	rm $(result) $(objects)
