#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../lib/command.h"

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
        // Additional alias handling logic can be added here
    }

    // Parse arguments and check for background/redirect flags
    for (int i = 0; i < tokenCount; ++i) {
        if (strcmp(tokens[i], "&") == 0) {
            cmd.background = 1;
            continue; // Skip the '&' token and continue processing
        }

        if (i < tokenCount - 1) { // Check next token for redirection
            if (strcmp(tokens[i], ">") == 0) {
                cmd.redirect = OUTPUT;
                cmd.arguments[cmd.num_arguments++] = tokens[i + 1]; // Next token is the file name
                i++; // Skip the next token as it's already processed
                continue;
            } else if (strcmp(tokens[i], ">>") == 0) {
                cmd.redirect = APPEND;
                cmd.arguments[cmd.num_arguments++] = tokens[i + 1];
                i++;
                continue;
            } else if (strcmp(tokens[i], ">>>") == 0) {
                cmd.redirect = REVERSE;
                cmd.arguments[cmd.num_arguments++] = tokens[i + 1];
                i++;
                continue;
            }
        }

        // Regular argument
        cmd.arguments[cmd.num_arguments++] = tokens[i];
    }

    return cmd;
}


void print_command(command cmd) {
    printf("Operation: %d\n", cmd.op);
    printf("Arguments (%d): ", cmd.num_arguments);
    for (int i = 0; i < cmd.num_arguments; ++i) {
        printf("%s ", cmd.arguments[i]);
    }
    printf("\nBackground: %d\n", cmd.background);
    printf("Redirect: %d\n", cmd.redirect);
}

