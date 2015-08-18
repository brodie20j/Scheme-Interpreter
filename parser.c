#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "parse.h"


int main(int argc, char *argv[]) {
    ConsCell *tokens;
    ConsCell *tree = malloc(sizeof(ConsCell));
    Value *treeCar = malloc(sizeof(Value));
    Value *treeCdr = malloc(sizeof(Value));
    treeCar->type = 0;
    treeCar->intValue = 0;
    treeCdr->type = 0;
    treeCdr->intValue = 0;
    tree = insertCC(tree, treeCar, treeCdr);


    int depth = 0;
    char *expression = calloc(sizeof(char), 256);
    while (fgets(expression, 256, stdin)) {
        tokens=tokenize(expression);
        ConsCell* headToken = tokens;
        PrintTokens(tokens);
        while (tokens->car->type !=0){
        	Value *token;
        	token = popFromFront(tokens, token);
        	tokens = changeTokens(tokens);
        	tree=addToParseTree(tree, token, &depth);
            if (depth < 0) {
                SyntaxError();
                depth = 0;
                break;
            }
        	else if ((depth == 0) && (tokens->car->type== 0)) {
        		printTree(tree);
                printf("\n");
                freeTree(tree);
                tree = malloc(sizeof(ConsCell));
                treeCar = malloc(sizeof(Value));
                treeCdr = malloc(sizeof(Value));
                treeCar->type = 0;
                treeCar->intValue = 0;
                treeCdr->type = 0;
                treeCdr->intValue = 0;
                tree = insertCC(tree, treeCar, treeCdr);
        		}
            else if ((depth == 0) && (tokens->car->type!= 0)){
                SyntaxError();
                depth = 0;
                break;
            }
                
        	}


        cleanupCCLL(headToken);


        

        
        
    }
    if (depth != 0) {
        SyntaxError();
    }
    free(expression);    
    freeTree(tree);
    return 0;
    /* also free anything you need to free in the list of tokens */
}