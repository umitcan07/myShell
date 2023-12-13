#define MAX_ARGUMENTS 256

typedef enum operation { NO_OP,
                         EXIT,
                         ALIAS,
                         OTHER } operation;

typedef enum redirect { NO_REDIRECT,
                        OUTPUT,
                        APPEND,
                        REVERSE } redirect;

typedef struct command {
    operation op;
    char *arguments[MAX_ARGUMENTS];
    int num_arguments;
    int background;
    redirect redirect;
} command;

command parse_command(char *tokens[], int tokenCount);
void print_command(command cmd);