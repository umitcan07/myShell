# Default target for compilation
default:
	gcc src/bello.c -o bin/bello
	gcc src/main.c src/tokenize.c src/alias.c -o ./myshell

# Run target for executing the program after compilation
run: default
	./myshell

# A clean target is also useful for removing compiled binaries
clean:
	rm -f bin/bello ./myshell

.PHONY: default run clean