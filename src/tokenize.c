#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/tokenize.h"

/* Function:  tokenize
 * --------------------
 * Tokenizes the input string into tokens. The tokens are stored in the tokens
 * array.
 *
 * input: the string to tokenize
 * tokens: the array to store the tokens in
 *
 * returns: the number of tokens
 */
int tokenize(char *input, char *tokens[MAX_TOKENS]) {
    int tokenCount = 0;
    int inQuotes = 0;
    char *token = malloc(MAX_TOKEN_LENGTH);
    int tokenIndex = 0;
    int length = strlen(input);

    if (!token) {
        return -1; // Memory allocation failed
    }

    for (int i = 0; i < length; i++) {
        if (input[i] == '"') {
            inQuotes = !inQuotes;
            if (!inQuotes) {
                token[tokenIndex] = '\0';
                tokens[tokenCount++] = token;
                token = malloc(MAX_TOKEN_LENGTH);
                tokenIndex = 0;
                if (!token) {
                    return -1; // Memory allocation failed
                }
            }
            continue;
        }

        if (input[i] == ' ' && !inQuotes) {
            if (tokenIndex != 0) {
                token[tokenIndex] = '\0';
                tokens[tokenCount++] = token;
                token = malloc(MAX_TOKEN_LENGTH);
                tokenIndex = 0;
                if (!token) {
                    return -1; // Memory allocation failed
                }
            }
        } else {
            if (tokenIndex < MAX_TOKEN_LENGTH - 1) {
                token[tokenIndex++] = input[i];
            }
        }
    }

    if (tokenIndex != 0) {
        token[tokenIndex] = '\0';
        tokens[tokenCount++] = token;
    } else {
        free(token);
    }

    tokens[tokenCount] = NULL;

    return tokenCount;
}

/* Function:  print_tokens
 * --------------------
 * Prints the tokens in the tokens array.
 *
 * tokens: the array of tokens to print
 * tokenCount: the number of tokens in the array
 *
 * returns: void
 */
void print_tokens(char *tokens[MAX_TOKENS], int tokenCount) {
    for (int i = 0; i < tokenCount; i++) {
        printf("Token[%d]: %s\n", i, tokens[i]);
    }
}