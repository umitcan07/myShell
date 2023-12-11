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
#include <limits.h>

#include "alias.h"
#include "tokenize.h"
#include "command.h"

#define MAX_INPUT_LENGTH 256

int add_directory_to_path(char *cwd, char *directory);



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

    // Last executed command string
    char *last_command = NULL;

    // Add bin directory to PATH
    add_directory_to_path(argv[0], "bin");

    // Main loop for the commands
    while (1) {
        // Prompt string: username@hostname:cwd ---
        printf("%s@%s %s --- ", username, hostname, cwd);

        // Get input and remove trailing newline
        if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) {
            printf("\n");
            break; // Exit on EOF
        }
        input[strlen(input) - 1] = '\0';

        // Tokenize input
        int tokenCount = tokenize(input, tokens);
        if (tokenCount == 0) {
            continue;
        }

        // print_tokens(tokens, tokenCount);

        // Check for exit command
        if (strcmp(tokens[0], "exit") == 0) {
            break;
        }

        // Check for alias command
        if (strcmp(tokens[0], "alias") == 0) {
            handle_alias_command(tokens, tokenCount);
            continue;
        } else {

            pid_t pid = fork();

            if(pid == 0) {
                // Child process
                if(strcmp(tokens[tokenCount - 1], "&") == 0) {
                    // Run in background
                    tokens[tokenCount - 1] = NULL;
                }
                if(execvp(tokens[0], tokens) == -1) {
                    printf("Error command '%s' not found\n", tokens[0]);
                    break;
                }
            }

            else {
                // Parent process
                int status;

                if(strcmp(tokens[tokenCount - 1], "&") == 0) {
                    // Run in background
                    printf("Running in background\n");
                } else {
                    // Run in foreground
                    waitpid(pid, &status, 0);
                }

            }


        }

        // Update the last_command
        last_command = input;

    }

    return 0;
}



/* Function:  add_directory_to_path
 * --------------------
 * Adds a specified directory, relative to the current working directory, to the PATH environment variable.
 * 
 * cwd: The current working directory. This should be provided without a leading slash.
 * directory: The directory to be added to PATH. This should be provided without a leading slash.
 *            The directory is appended to the current working directory with an intervening slash.
 * 
 * Example Usage:
 *     - If the current working directory is /home/user/project and the directory parameter is "bin",
 *       then /home/user/project/bin will be added to PATH.
 *     - Pass the directory parameter without a leading slash, e.g., "bin", not "/bin".
 *
 * returns: 0 if successful, 1 if not (e.g., due to errors in retrieving the current working directory or setting the environment variable).
 */
int add_directory_to_path(char *cwd, char *directory) {
    char *path = getenv("PATH"); // Get the current PATH
    if (path == NULL) {
        printf("Error: Unable to retrieve PATH environment variable.\n");
        return 1;
    }

    // Calculate the required buffer size
    int requiredSize = strlen(cwd) + strlen(directory) + strlen(path) + 3; // +3 for the slash, colon, and null terminator

    // Allocate memory for the new PATH
    char *newPath = (char *)malloc(requiredSize * sizeof(char));
    if (newPath == NULL) {
        printf("Error: Memory allocation failed.\n");
        return 1;
    }

    // Construct the new directory path and append it to the PATH
    sprintf(newPath, "%s:%s/%s", path, cwd, directory); // Format: PATH:cwd/directory

    // Set the new PATH
    if (setenv("PATH", newPath, 1) != 0) {
        printf("Error: Unable to set PATH environment variable.\n");
        free(newPath);
        return 1;
    }

    // Debugging purposes
    // printf("New PATH: %s\n", newPath);

    free(newPath); 
    return 0; 
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
command *create_command(char *tokens[MAX_TOKENS], int tokenCount) {
    command *cmd = malloc(sizeof(command));

    return cmd;
}