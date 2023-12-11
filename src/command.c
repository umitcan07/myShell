#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "command.h"

/* Command structure
 * Fields:
 * - operation: NONE | EXIT | ALIAS | OTHER
 * - arguments: char *[]
 * - num_arguments: int
 * - background: int (0 = false, 1 = true)
 * - redirect: NONE | OUTPUT | APPEND | REVERSE
 */

// #define MAX_ARGUMENTS 256

// typedef struct command {
//     operation op;
//     char *arguments[MAX_ARGUMENTS];
//     int num_arguments;
//     int background;
//     redirect redirect;
// } command;

// typedef enum operation {
//     NONE,
//     EXIT,
//     ALIAS,
//     OTHER
// } operation;

// typedef enum redirect {
//     NONE,
//     OUTPUT,
//     APPEND,
//     REVERSE
// } redirect;

command parse_command(char *tokens[], int tokenCount) {
    command cmd;
    cmd.op = OTHER;
    cmd.num_arguments = 0;
    cmd.background = 0;
    cmd.redirect = NONE;

    // Check for empty command
    if (tokenCount == 0) {
        return cmd;
    }

    // Check for exit command
    if (strcmp(tokens[0], "exit") == 0) {
        cmd.op = EXIT;
        return cmd;
    }

    // Check for alias command
    if (strcmp(tokens[0], "alias") == 0) {
        cmd.op = ALIAS;
        return cmd;
    }

    // Check for background command
    if (strcmp(tokens[tokenCount - 1], "&") == 0) {
        cmd.background = 1;
        tokenCount--;
    }

    // Check for redirect command
    if (strcmp(tokens[tokenCount - 2], ">") == 0) {
        cmd.redirect = OUTPUT;
        tokenCount -= 2;
    } else if (strcmp(tokens[tokenCount - 2], ">>") == 0) {
        cmd.redirect = APPEND;
        tokenCount -= 2;
    } else if (strcmp(tokens[tokenCount - 2], ">>>") == 0) {
        cmd.redirect = REVERSE;
        tokenCount -= 2;
    }

    // Copy arguments into command structure
    for (int i = 0; i < tokenCount; i++) {
        cmd.arguments[i] = tokens[i];
        cmd.num_arguments++;
    }

    return cmd;
}
