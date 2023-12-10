/* Custom shell with most of the features of a standard shell. 
 * The shell should be able to execute commands in foreground & background, create aliases, and redirect input and output.
 * 
 * The prompt string should be of the form: 
 * username@hostname:cwd ---
 * 
 * The shell should support the following special operations:
 * > - redirect output to a file (overwrite)
 * >> - redirect output to a file (append)
 * >>> - redirect output to a file (append, but invert the order of all letters in the output)
 * & - run the command in the background
 * 
 * Other specifications:
 * It works in a fork-exec manner.
 * It can run each and every command in the PATH environment variable.
 * In case of a collision between an alias and a command, the alias should take precedence.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

// int create_alias(char *alias_name, char *alias_command);
#include "alias.h"
// int create_alias(char *alias_name, char *alias_command);
#include "tokenize.h"
// int tokenize(char *input, char *tokens[MAX_TOKENS])
#include "command.h"

#define MAX_INPUT_LENGTH 256

void print_tokens(char *tokens[MAX_TOKENS], int tokenCount);

int main(int argc, char **argv) {
    // Initialize variables for input and tokens
    char input[MAX_TOKEN_LENGTH];
    char *tokens[MAX_TOKENS];

    // Get current working directory, hostname, and username
    char cwd[256];
    getcwd(cwd, sizeof(cwd));

    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    
    char *username = getenv("USER");

    // Main loop for the commands
    while (1) {
        // Prompt string: username@hostname:cwd ---
        printf("%s@%s %s --- ", username, hostname, cwd);

        // Get input
        fgets(input, MAX_INPUT_LENGTH, stdin);

        // Remove trailing newline
        input[strlen(input) - 1] = '\0';

        // Tokenize input
        int tokenCount = tokenize(input, tokens);

        // Check for exit command
        if (strcmp(tokens[0], "exit") == 0) {
            break;
        }

        if (strcmp(tokens[0], "alias") == 0) {
            if (tokenCount != 4) {
                printf("Error: Invalid number of arguments for 'alias' command.\n");
                continue;
            }

            char *alias_name = tokens[1];
            char *alias_command = tokens[3];

            int result = create_alias(alias_name, alias_command);
            if (result == 0) {
                printf("Alias created successfully.\n");
            } else {
                printf("Error: Failed to create alias.\n");
            }
            continue;
        }

    }

    return 0;
}

/* Function:  print_tokens
 * --------------------
 * Prints the tokens in the tokens array.
 * 
 * tokens: the array of tokens to print
 * tokenCount: the number of tokens in the array
 * 
 * returns: void
 */
void print_tokens(char *tokens[MAX_TOKENS], int tokenCount) {
    for (int i = 0; i < tokenCount; i++) {
        printf("%s\n", tokens[i]);
    }
}

/* Function:  create_command
 * --------------------
 * Creates a command struct from the tokens.
 * 
 * tokens: the array of tokens to create the command from
 * tokenCount: the number of tokens in the array
 * 
 * returns: a pointer to the command struct
 */



// #define MAX_ARGUMENTS 256

// typedef struct command {
//     int operation;
//     char *arguments[MAX_ARGUMENTS];
//     int num_arguments;
//     int background;
//     int redirect;
// } command;