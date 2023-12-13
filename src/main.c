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
#include <ctype.h>
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

#define MAX_INPUT_LENGTH 512
#define MAX_PATH_LENGTH 512

int add_directory_to_path(char *directory);
int save_history(char *last_command);
char *find_executable(char *command);
void get_alias(const char *alias_name, char *buffer, size_t buffer_size);
void get_alias(const char *alias_name, char *buffer, size_t buffer_size);
void replace_alias_in_command(char *input, char *output, size_t max_output_length);

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

        // Keep a temporary copy of the input just to keep a record of last executed command
        char temp_input[MAX_INPUT_LENGTH];
        strcpy(temp_input, input);

        // Before tokenizing the input, check if it is an alias.
        // Get the first token (do not consider qoutes, they are not part of the
        // alias name. i.e. take the first word)
        // If it is an alias, replace the alias name with the alias value

        char output[MAX_INPUT_LENGTH];

        replace_alias_in_command(input, output, MAX_INPUT_LENGTH);
        // Tokenize input
        printf("Input: %s\n", input);
        printf("Replaced command: %s\n", output);

        int tokenCount = tokenize(output, tokens);
        if (tokenCount == 0) {
            continue;
        }

        // For debugging purposes
        // print_tokens(tokens, tokenCount);

        command cmd = parse_command(tokens, tokenCount);

        // For debugging purposes
        print_command(cmd);

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

                // Get the output file, if any, to be right after the >, >> or
                // >>> operator

                char *output_file = NULL;
                for (int i = 0; i < tokenCount; i++) {
                    if (strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], ">>") == 0 || strcmp(tokens[i], ">>>") == 0) {
                        output_file = tokens[i + 1];
                        break;
                    }
                }

                FILE *fp = NULL;

                // If the command is to be redirected to a file
                if (output_file != NULL) {
                    if (cmd.redirect == 1) {
                        // Redirect
                        fp = fopen(output_file, "w");
                        if (fp == NULL) {
                            printf("Error: Unable to open file for "
                                   "redirecting.\n");
                            exit(1);
                        }

                        // Redirect stdout to the file
                        dup2(fileno(fp), STDOUT_FILENO);
                        fclose(fp);
                    } else if (cmd.redirect == 2) {
                        // Append
                        fp = fopen(output_file, "a");
                        if (fp == NULL) {
                            printf("Error: Unable to open file for appending.\n");
                            exit(1);
                        }
                        // Redirect stdout to the file
                        dup2(fileno(fp), STDOUT_FILENO);
                        fclose(fp);
                    } else if (cmd.redirect == 3) {
                        // Append, but invert the order of all letters in the
                        // output
                        fp = fopen(output_file, "a");
                        if (fp == NULL) {
                            printf("Error: Unable to open file for reverse redirecting.\n");
                            exit(1);
                        }

                        // Redirect stdout to the file
                        dup2(fileno(fp), STDOUT_FILENO);
                    }

                    // Redirect stdout to the file
                    dup2(fileno(fp), STDOUT_FILENO);
                    fclose(fp);
                }
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

        // Save the last executed command
        last_command = strdup(temp_input);
        save_history(last_command);
    }

    // Free the last command
    free(last_command);

    // Initialize the .history file as empty
    FILE *fp = fopen(".history", "w");
    if (fp == NULL) {
        printf("Error: Unable to open history file.\n");
        return 1;
    }
    fclose(fp);

    return 0;
}

/* Function:  add_directory_to_path
 * --------------------
 * Adds a specified directory, relative to the current working directory, to
 * the PATH environment variable.
 *
 * cwd: The current working directory. This should be provided without a
 * leading slash. directory: The directory to be added to PATH. This should
 * be provided without a leading slash. The directory is appended to the
 * current working directory with an intervening slash.
 *
 * Example Usage:
 *     - If the current working directory is /home/user/project and the
 * directory parameter is "bin", then /home/user/project/bin will be added
 * to PATH.
 *     - Pass the directory parameter without a leading slash, e.g., "bin",
 * not
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
    int requiredSize = strlen(cwd) + strlen(directory) + strlen(path) + 3; // +3 for the slash, colon, and null terminator

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

    // For debugging purposes
    // printf("New PATH: %s\n", newPath);

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

void trim_whitespace(char *str) {
    if (str == NULL)
        return;

    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0) { // All spaces?
        return;
    }

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    // Write new null terminator
    *(end + 1) = 0;
}

void get_alias(const char *alias_name, char *buffer, size_t buffer_size) {
    FILE *file = fopen(".aliases", "r");
    if (!file) {
        perror("Error opening .aliases file");
        return;
    }

    char line[MAX_ALIASES];
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");

        if (key) {
            trim_whitespace(key);
        }
        if (value) {
            trim_whitespace(value);
        }

        if (key && value && strcmp(key, alias_name) == 0) {
            strncpy(buffer, value, buffer_size);
            buffer[buffer_size - 1] = '\0'; // Ensure null termination
            found = 1;
            break;
        }
    }

    if (!found) {
        buffer[0] = '\0'; // Set buffer to empty string if alias not found
    }

    fclose(file);
}

void replace_alias_in_command(char *input, char *output, size_t max_output_length) {
    char temp_input[MAX_INPUT_LENGTH];
    strncpy(temp_input, input, MAX_INPUT_LENGTH);
    temp_input[MAX_INPUT_LENGTH - 1] = '\0'; // Ensure null termination

    char *alias_name = strtok(temp_input, " ");
    char alias_value[MAX_INPUT_LENGTH] = {0};

    if (alias_name) {
        get_alias(alias_name, alias_value, sizeof(alias_value));
        if (strlen(alias_value) > 0) {
            // Alias found
            snprintf(output, max_output_length, "%s", alias_value);
            char *remainder = input + strlen(alias_name);
            while (*remainder == ' ')
                remainder++; // Skip spaces to find the start of the next token

            if (*remainder != '\0') {
                // Ensure we don't exceed buffer size
                size_t current_length = strlen(output);
                snprintf(output + current_length, max_output_length - current_length, " %s", remainder);
            }
        } else {
            // Alias not found, copy original input
            strncpy(output, input, max_output_length);
            output[max_output_length - 1] = '\0'; // Ensure null termination
        }
    } else {
        // Input was only whitespace or empty
        strncpy(output, input, max_output_length);
        output[max_output_length - 1] = '\0'; // Ensure null termination
    }
}