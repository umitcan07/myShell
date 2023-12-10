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

    // Add /bin to the PATH environment variable
    add_directory_to_path("cwd", "bin");

    // Main loop for the commands
    while (1) {
        // Prompt string: username@hostname:cwd ---
        printf("%s@%s %s --- ", username, hostname, cwd);

        // Get input and remove trailing newline
        fgets(input, MAX_INPUT_LENGTH, stdin);
        input[strlen(input) - 1] = '\0';

        // Tokenize input
        int tokenCount = tokenize(input, tokens);

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
                // If not an alias, check for redirection
                execvp(tokens[0], tokens);
            } else {
                // Parent process
                wait(NULL);
            }


        }

        



    }

    return 0;
}



/* Function:  add_directory_to_path
 * --------------------
 * Adds a specified directory, relative to the current working directory, to the PATH environment variable.
 * 
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
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char newPath[PATH_MAX];
        snprintf(newPath, sizeof(newPath), "%s/%s:%s", cwd, directory, getenv("PATH"));

        if (setenv("PATH", newPath, 1) != 0) {
            perror("setenv error");
            return 1;
        }

    } else {
        perror("getcwd() error");
        return 1;
    }

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