// William Schifeling and Jonathan Brodie, Zach Rubin
// I talked to colby and garo and nick jones and david... Much of the code is from Dave Musicant.
#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"

int main(int argc, char *argv[]) {
    ConsCell *tokens;
    char *expression = calloc(sizeof(char), 256);
   	//FILE *fp=fopen("tester.txt", "r");
    while (fgets(expression, 256, stdin)) {
        tokens=tokenize(expression);
        PrintTokens(tokens);
        cleanupCCLL(tokens);
    }
    
    free(expression);
    

    return 0;
    /* also free anything you need to free in the list of tokens */
}

