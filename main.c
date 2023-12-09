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
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_COMMAND_LENGTH 256
#define MAX_ARGUMENTS 256
#define MAX_ALIASES 256


/*
 * Function:  create_alias
 * --------------------
 * Creates an alias for a command. The alias is recorded in a file called .aliases in the current
 * directory. The format of the file is as follows:
 * alias_name = alias_command
 * If the file already exists, the new alias should be appended to the end of the file.
 * If the file does not exist, it should be created.
 * 
 * If alias_name is already defined, the new alias should overwrite the old one.
 * 
 * Assum alias_command is always provided within double quotes.
 *
 * alias_name: the name of the alias
 * alias_command: the command that the alias is for
 *
 * returns: 0 if the alias is created successfully, 1 otherwise
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int create_alias(char *alias_name, char *alias_command) {
    FILE *file;
    char line[256];
    int found = 0;
    char temp_filename[] = ".aliases_temp";

    // Open the .aliases file for reading
    file = fopen(".aliases", "r");
    if (file == NULL) {
        file = fopen(".aliases", "w"); // Create the file if it does not exist
        if (file == NULL) {
            printf("Error: Failed to create .aliases file.\n");
            return 1;
        }
    }
    fclose(file);

    // Create a temporary file for writing
    FILE *temp_file = fopen(temp_filename, "w");
    if (temp_file == NULL) {
        printf("Error: Failed to create temporary file.\n");
        return 1;
    }

    // Re-open .aliases for reading
    file = fopen(".aliases", "r");
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, alias_name, strlen(alias_name)) == 0 && line[strlen(alias_name)] == '=') {
            found = 1; // Mark the alias as found
            fprintf(temp_file, "%s = %s\n", alias_name, alias_command); // Overwrite the alias
        } else {
            fprintf(temp_file, "%s", line); // Copy the line to the temp file
        }
    }

    // If the alias was not found, append it
    if (!found) {
        fprintf(temp_file, "%s = %s\n", alias_name, alias_command);
    }

    fclose(file);
    fclose(temp_file);

    // Replace the original .aliases file with the temp file
    if (remove(".aliases") != 0 || rename(temp_filename, ".aliases") != 0) {
        printf("Error: Failed to update .aliases file.\n");
        return 1;
    }

    return 0;
}

/*
 * Function:  traverse_environment_path
 * --------------------
 * Searches for a command in the directories listed in the PATH environment variable.
 * If the command is found, the shell should execute it.
 * If the command is not found, the shell should print an error message and wait for the next command.
 *
 * command_name: the name of the command to search for
 *
 * returns: 0 if the command is found, 1 otherwise
 */

int traverse_environment_path(char *command_name) {
    return 0;
}


int main(int argc, char **argv) {
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
        // Prompt string: username@hostname:cwd ---
        printf("%s@%s ~%s --- ", username, hostname, cwd);

        // Get command from stdin
        fgets(command, MAX_COMMAND_LENGTH, stdin);
        command[strlen(command) - 1] = '\0';

        // Tokenize command into arguments
        token = strtok(command, " ");
        i = 0;
        while (token != NULL) {
            arguments[i++] = token;
            token = strtok(NULL, " ");
        }
        arguments[i] = NULL;


        // Print arguments
        // Possible special characters: >, >>, &, >>>
        // A new re-redirection operator appears ("> > >") (no spaces). This operator will
        // act exactly as ("> >"), but the order of all letters in the output will be invested!
        // Please refer to the example below.

        if (strcmp(arguments[0], "exit") == 0) {
            // Exit the shell
            return 0;
        }

        // for(int j = 0; j < i; j++) {
        //     printf("%s\n", arguments[j]);
        // }

        // Create alias if the command is "alias"

        if (strcmp(arguments[0], "alias") == 0) {
            // Create alias
            char *alias_name = arguments[1];
            char *alias_command = arguments[3];
            for(int j = 4; j < i; j++) {
                strcat(alias_command, " ");
                strcat(alias_command, arguments[j]);
            }
            create_alias(alias_name, alias_command);
            continue;
        }


        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            execvp(arguments[0], arguments);

            // Failed to execute command
            printf("Error: command not found.\n");
            return 1;

        } else if (pid < 0) {
            // Fork failed
            printf("Error: fork failed.\n");
            return 1;
        }
        else {
            // Parent process
            wait(NULL);
        }
    }
    return 0;
}