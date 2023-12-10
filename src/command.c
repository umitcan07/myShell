#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "command.h"

/* Command structure
 * Fields:
 * - operation: int (0 = exit, 1 = alias, 2 = other)
 * - arguments: char *[]
 * - num_arguments: int
 * - background: int (0 = false, 1 = true)
 * - redirect: int (0 = none, 1 = output, 2 = append, 3 = reverse)
 */

// #define MAX_ARGUMENTS 256

// typedef struct command {
//     int operation;
//     char *arguments[MAX_ARGUMENTS];
//     int num_arguments;
//     int background;
//     int redirect;
// } command;

/* Function: parse_command
 * -----------------------
 * Parses a command from a list of tokens.
 * 
 */

