// Custom Shell

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGUMENTS 64

int main(int argc, char **argv) {
    if (isatty(fileno(stdin))) {
        char command[MAX_COMMAND_LENGTH];
        char *arguments[MAX_ARGUMENTS];
        char *token;
        int i;

        // Get current working directory, hostname, and username
        char cwd[256];
        getcwd(cwd, sizeof(cwd));

        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        
        char *username = getenv("USER");

        // Main loop for the commands
        while (1) {
            // Promt string: username@hostname:cwd ~
            printf("%s@%s ~%s --- ", username, hostname, cwd);

            // Get command from stdin
            fgets(command, MAX_COMMAND_LENGTH, stdin);
            command[strlen(command) - 1] = '\0';

            if (strcmp(command, "exit") == 0) {
                break;
            }

            token = strtok(command, " ");
            i = 0;
            while (token != NULL) {
                arguments[i++] = token;
                token = strtok(NULL, " ");
            }
            arguments[i] = NULL;

            if (fork() == 0) {
                // Child process
                execvp(arguments[0], arguments);
                printf("Error: command not found.\n");
                return 1;
            }
            else {
                wait(NULL);
            }
        }
        return 0;
    }
    else {
        printf("Error: stdin is not a terminal.\n");
        return 1;
    }
}