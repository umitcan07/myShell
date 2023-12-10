#include "alias.h"

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
 * 
 * Examples:
 * alias x = "echo hello" > .aliases > x = echo hello
 * alias x = y > .aliases > x = y
 */
int create_alias(char *alias_name, char *alias_command) {
    FILE *file;
    char line[1024];
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
        char existing_alias[256];
        sscanf(line, "%s =", existing_alias); // Extract the alias name from the line

        if (strcmp(existing_alias, alias_name) == 0) {
            fprintf(temp_file, "%s = %s\n", alias_name, alias_command); // Overwrite the alias
            found = 1;
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
