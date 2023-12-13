# myshell

Ümit Can Evleksiz (2020400114)

## Introduction

`make` to compile the program.
`./myshell` to run the program.
Alternatively `make run` to compile and run the program.

## Features

- The prompt string: `username@hostname:cwd ---`
- `>` - redirect output to a file (overwrite)
- `>>` - redirect output to a file (append)
- `>>>` - redirect output to a file (append, but invert the order of all letters in the output)
- `&` - run the command in the background
- `alias x = y` - create an alias for the command y, named x

### Some Design Decisions and Specifications

- It works in a fork-exec manner, meaning that it creates a child process for each command.
- It can run each and every command in the PATH environment variable.
- In case of a collision between an alias and a command, the alias should take precedence.
- Use of getcwd() as cwd for the prompt string. This is done to make sure that the prompt string is always up to date.
- Process count takes into account the zombie processes as well since they are not waited for.
- Last executed command resolves into a raw command from the user, including all the arguments. (i.e. input: `ls -l >> a.txt`, output: `ls -l >> a.txt`)
- Background processing yields prompt string to be printed before the command is finished executing, similar to how bash handles.
- Alias resolves into corresponding command and arguments while right after getting the input from the user. (i.e. input: `ls -l`, alias: `ls = ls -a`, output: `ls -l -a`)

### Difficulties Encountered

- Whether or not tokenize anything in double quotes as a single token/word.
- Background processing, zombie processes, and process count.
- Resolving aliases while tokenizing the input from the user.

### Functional Limitations

-

### Variable Limitations (`#define`d MACROS)

- MAX_LINE_LENGTH: 1024
- MAX_TOKEN_COUNT: 256
- MAX_TOKEN_LENGTH: 256
