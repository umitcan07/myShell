SRC = ${wildcard src/*.c}

# Default target for compilation
default: $(SRC)
	mkdir -p bin
	gcc-13 src/bello/bello.c -o bin/bello
	gcc-13 $^ -o ./myshell

# Run target for executing the program after compilation
run: default
	./myshell

# A clean target is also useful for removing compiled binaries
clean:
	rm -f bin/bello ./myshell
	rm -f .history .aliases
	rm -rf bin

.PHONY: default run clean