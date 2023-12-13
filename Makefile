SRC = ${wildcard src/*.c}

# Default target for compilation
default: $(SRC)
	gcc-13 src/bello/bello.c -o bin/bello
	gcc-13 $^ -o ./myshell

# Run target for executing the program after compilation
run: default
	./myshell

# A clean target is also useful for removing compiled binaries
clean:
	rm -f bin/bello ./myshell

.PHONY: default run clean