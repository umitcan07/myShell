/* Command structure
 * Fields:
 * - operation: NONE | EXIT | ALIAS | OTHER
 * - arguments: char *[]
 * - num_arguments: int
 * - background: int (0 = false, 1 = true)
 * - redirect: NONE | OUTPUT | APPEND | REVERSE
 */

#define MAX_ARGUMENTS 256

typedef enum operation {
    NO_OP,
    EXIT,
    ALIAS,
    OTHER
} operation;

typedef enum redirect {
    NO_REDIRECT,
    OUTPUT,
    APPEND,
    REVERSE
} redirect;

typedef struct command {
    operation op;
    char *arguments[MAX_ARGUMENTS];
    int num_arguments;
    int background;
    redirect redirect;
} command;

command parse_command(char *tokens[], int tokenCount);