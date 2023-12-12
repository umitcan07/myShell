#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_ALIASES 256

int create_alias(char *alias_name, char *alias_command);
int handle_alias_command(char **tokens, int tokenCount);