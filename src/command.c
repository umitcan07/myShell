#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/command.h"

/* Function: parse_command
 * -----------------------
 * Parses a list of tokens into a command struct.
 *
 * tokens: A list of tokens to parse.
 * tokenCount: The number of tokens in the list.
 *
 * returns: A command struct representing the parsed command.
 */
command parse_command(char *tokens[], int tokenCount) {
    command cmd;
    cmd.op = OTHER; // Default to OTHER for regular commands
    cmd.num_arguments = 0;
    cmd.background = 0;
    cmd.redirect = NO_REDIRECT;

    // Early exit for empty command
    if (tokenCount == 0) {
        cmd.op = NO_OP;
        return cmd;
    }

    // Handle special commands
    if (strcmp(tokens[0], "exit") == 0) {
        cmd.op = EXIT;
    } else if (strcmp(tokens[0], "alias") == 0) {
        cmd.op = ALIAS;
    }

    // Parse arguments and check for background/redirect flags
    for (int i = 0; i < tokenCount; ++i) {
        if (strcmp(tokens[i], "&") == 0) {
            cmd.background = 1;
            continue; // Skip the '&' token
        }

        // Check and handle redirection operators
        if (i < tokenCount - 1 &&
            (strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], ">>") == 0 ||
             strcmp(tokens[i], ">>>") == 0)) {
            // Set the appropriate redirect type
            if (strcmp(tokens[i], ">") == 0) {
                cmd.redirect = OUTPUT;
            } else if (strcmp(tokens[i], ">>") == 0) {
                cmd.redirect = APPEND;
            } else if (strcmp(tokens[i], ">>>") == 0) {
                cmd.redirect = REVERSE;
            }
            i++; // Skip the next token (filename for redirection)
            continue;
        }

        // Regular argument
        cmd.arguments[cmd.num_arguments++] = tokens[i];
    }

    return cmd;
}

/* Function: print_command
 * -----------------------
 * Prints the contents of a command struct.
 *
 * cmd: The command struct to print.
 */
void print_command(command cmd) {
    printf("Operation: %d\n", cmd.op);
    printf("Arguments (%d): ", cmd.num_arguments);
    for (int i = 0; i < cmd.num_arguments; ++i) {
        printf("%s ", cmd.arguments[i]);
    }
    printf("\nBackground: %d\n", cmd.background);
    printf("Redirect: %d\n", cmd.redirect);
}
