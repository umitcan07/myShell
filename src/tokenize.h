#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TOKENS 256
#define MAX_TOKEN_LENGTH 256

int tokenize(char *input, char *tokens[MAX_TOKENS]);