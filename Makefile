SRC = ${wildcard src/*.c}

# Default target for compilation
default: $(SRC)
	gcc $^ -o ./myshell

# Run target for executing the program after compilation
run: default
	./myshell

# A clean target is also useful for removing compiled binaries
clean:
	rm -f bin/bello ./myshell

.PHONY: default run clean