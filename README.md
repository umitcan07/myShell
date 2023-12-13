# myshell

## Description

This is a simple shell written in C.

## Features

- The prompt string: `username@hostname:cwd ---`
- `>` - redirect output to a file (overwrite)
- `>>` - redirect output to a file (append)
- `>>>` - redirect output to a file (append, but invert the order of all letters in the output)
- `&` - run the command in the background
- `alias x = y` - create an alias for the command y, named x
- `bello` - run the bello program

* Bello Program: Displays various information about the user and system:

1.  Username: Retrieved using getenv("USER").
2.  Hostname: Retrieved using gethostname().
3.  Last Executed Command: Read from a file called .history
4.  TTY
5.  Current Shell Name
6.  Home Location
7.  Current Time and Date
8.  Current Number of Processes: Retrieved by executing the 'ps -e' command and counting lines of output.

### Some Design Decisions and Specifications

- It works in a fork-exec manner, meaning that it creates a child process for each command.
- It can run each and every command in the PATH environment variable.
- In case of a collision between an alias and a command, the alias should take precedence.
- Use of getcwd() as cwd for the prompt string. This is done to make sure that the prompt string is always up to date.
- Process count takes into account the zombie processes as well since they are not waited for.
- Last executed command resolves into a raw command from the user, including all the arguments. (i.e. input: `ls -l >> a.txt`, output: `ls -l >> a.txt`)
- Background processing yields prompt string to be printed before the command is finished executing, similar to how bash handles.
- Alias resolves into corresponding command and arguments while right after getting the input from the user. (i.e. input: `ls -l`, alias: `ls = ls -a`, output: `ls -l -a`)
- Bello functionality is provided as an executable file in the same directory as the myshell executable. After it is compiled with the same makefile, the directory `/bin` is added to the PATH. Therefore, whenever `bello` is called, there guaranteed to be at least 1 child process.
- Last executed command is stored in a file called `.history` in the same directory as the myshell executable. It is created if it does not exist. It is overwritten if it exists.

### Author

Umit Can Evleksiz - 2020400114
Bogazici University - Computer Engineering Department
