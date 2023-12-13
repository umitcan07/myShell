/* Custom shell with most of the features of a standard shell.
 * The shell should be able to execute commands in foreground & background,
 * create aliases, and redirect input and output.
 *
 * The prompt string should be of the form:
 * username@hostname:cwd ---
 *
 * The shell should support the following special operations:
 * > - redirect output to a file (overwrite)
 * >> - redirect output to a file (append)
 * >>> - redirect output to a file (append, but invert the order of all letters
 * in the output) & - run the command in the background
 *
 * Other specifications:
 * It works in a fork-exec manner.
 * It can run each and every command in the PATH environment variable.
 * In case of a collision between an alias and a command, the alias should take
 * precedence.
 */
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "../lib/alias.h"
#include "../lib/command.h"
#include "../lib/tokenize.h"

#define MAX_INPUT_LENGTH 256
#define MAX_PATH_LENGTH 256

int add_directory_to_path(char *directory);
void signal_handler(int signum);
int save_history(char *last_command);
char *find_executable(char *command);

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
    add_directory_to_path("bin");

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
        // Keep a temporary copy of the input
        char temp_input[MAX_INPUT_LENGTH];
        strcpy(temp_input, input);

        // Tokenize input
        int tokenCount = tokenize(input, tokens);
        if (tokenCount == 0) {
            continue;
        }

        // For debugging purposes
        // print_tokens(tokens, tokenCount);

        command cmd = parse_command(tokens, tokenCount);
        print_command(cmd);

        // For debugging purposes
        // print_command(cmd);

        switch (cmd.op) {
        case NO_OP:
            break;

        case EXIT:
            printf("Exiting shell...\n");
            exit(0);
            break;

        case ALIAS:
            handle_alias_command(tokens, tokenCount);
            break;

        case OTHER: {

            char *executablePath;

            // Ensure the arguments array is null-terminated
            cmd.arguments[cmd.num_arguments] = NULL;

            executablePath = find_executable(cmd.arguments[0]);
            if (!executablePath) {
                printf("Command not found: %s\n", cmd.arguments[0]);
                break;
            }

            pid_t pid = fork();
            if (pid == 0) {
                // Child process
                execv(executablePath, cmd.arguments);
                perror("execv"); // If execv returns, there was an error
                exit(1);
            } else if (pid > 0) {
                // Parent process
                if (!cmd.background) {
                    int status;
                    waitpid(pid, &status, 0);
                }
            } else {
                perror("Fork failed");
            }
        } break;

        default:
            printf("Error: Invalid command.\n");
            break;
        }
    }

    return 0;
}

/* Function:  add_directory_to_path
 * --------------------
 * Adds a specified directory, relative to the current working directory, to the
 * PATH environment variable.
 *
 * cwd: The current working directory. This should be provided without a leading
 * slash. directory: The directory to be added to PATH. This should be provided
 * without a leading slash. The directory is appended to the current working
 * directory with an intervening slash.
 *
 * Example Usage:
 *     - If the current working directory is /home/user/project and the
 * directory parameter is "bin", then /home/user/project/bin will be added to
 * PATH.
 *     - Pass the directory parameter without a leading slash, e.g., "bin", not
 * "/bin".
 *
 * returns: 0 if successful, 1 if not (e.g., due to errors in retrieving the
 * current working directory or setting the environment variable).
 */
int add_directory_to_path(char *directory) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("Error getting current working directory");
        return 1;
    }

    char *path = getenv("PATH");
    if (path == NULL) {
        printf("Error: Unable to retrieve PATH environment variable.\n");
        return 1;
    }

    // Calculate the required buffer size
    int requiredSize = strlen(cwd) + strlen(directory) + strlen(path) +
                       3; // +3 for the slash, colon, and null terminator

    // Allocate memory for the new PATH
    char *newPath = (char *)malloc(requiredSize * sizeof(char));
    if (newPath == NULL) {
        printf("Error: Memory allocation failed.\n");
        return 1;
    }

    // Construct the new directory path and append it to the PATH
    sprintf(newPath, "%s:%s/%s", path, cwd,
            directory); // Format: PATH:cwd/directory

    // Set the new PATH
    if (setenv("PATH", newPath, 1) != 0) {
        printf("Error: Unable to set PATH environment variable.\n");
        free(newPath);
        return 1;
    }

    // printf("New PATH: %s\n", newPath); // For debugging purposes

    free(newPath);
    return 0;
}

/* Function: save_history
 * --------------------
 * Saves the last executed command to a file.
 *
 * last_command: The last executed command.
 *
 * returns: 0 if successful, 1 if not.
 */
int save_history(char *last_command) {
    FILE *fp = fopen(".history", "w");
    if (fp == NULL) {
        printf("Error: Unable to open history file.\n");
        return 1;
    }

    fprintf(fp, "%s\n", last_command);

    fclose(fp);
    return 0;
}

/* Function: find_executable
 * --------------------
 * Finds the full path to an executable in the PATH environment variable.
 *
 * command: The command to be searched for.
 *
 * returns: The full path to the executable if found, NULL otherwise.
 */
char *find_executable(char *command) {
    char *path = getenv("PATH");
    char *pathCopy = strdup(path);
    char *dir = strtok(pathCopy, ":");
    static char fullPath[MAX_PATH_LENGTH];

    while (dir != NULL) {
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dir, command);
        if (access(fullPath, X_OK) == 0) {
            free(pathCopy);
            return fullPath;
        }
        dir = strtok(NULL, ":");
    }

    free(pathCopy);
    return NULL;
}