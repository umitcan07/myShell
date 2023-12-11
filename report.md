# myshell
Ümit Can Evleksiz (2020400114)

## Introduction
`make` to compile the program.
`./myshell` to run the program.
Alternatively `make run` to compile and run the program.

## Features
 * The prompt string: `username@hostname:cwd ---`
 * `>` - redirect output to a file (overwrite)
 * `>>` - redirect output to a file (append)
 * `>>>` - redirect output to a file (append, but invert the order of all letters in the output)
 * `&` - run the command in the background
 * `alias x = y` - create an alias for the command y, named x


### Some Design Decisions
 * It works in a fork-exec manner, meaning that it creates a child process for each command.
 * It can run each and every command in the PATH environment variable.
 * In case of a collision between an alias and a command, the alias should take precedence.
 * Use of argv[0] as cwd to add to the PATH environment variable. This is done to make sure that the program can be run from anywhere.
 * Use of getcwd() as cwd for the prompt string. This is done to make sure that the prompt string is always up to date.

### Difficulties Encountered
 * Whether or not tokenize anything in double quotes as a single token/word.
 * Background processing.

### Limitations


