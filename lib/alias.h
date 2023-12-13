#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_ALIASES 512

int create_alias(char *alias_name, char *alias_command);
int handle_alias_command(char **tokens, int tokenCount);