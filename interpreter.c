#include <stdio.h>
#include <stdlib.h>
#include "interpret.c"



int main(int argc, char *argv[]) {
    freeCons = malloc(sizeof(ConsCell));// this is a global that is the head of our freeList.
    insertCC(freeCons, createNullVal(), createNullVal());
    
    ConsCell* cell=createEnvironment(); // initialize the environment
    ConsCell *tokens;
    ConsCell *tree = mallocConsCell();// initialize the tree.
    Value *treeCar = mallocValue();
    Value *treeCdr = mallocValue();
    treeCar->type = 0;
    treeCar->intValue = 0;
    treeCdr->type = 0;
    treeCdr->intValue = 0;
    tree = insertCC(tree, treeCar, treeCdr);
    int define = 0;

    int depth = 0;
    char *expression = calloc(sizeof(char), 256);
    while (fgets(expression, 256, stdin)) {
        tokens=tokenize(expression);
        ConsCell* headToken = tokens;
        free(expression);
        expression = calloc(sizeof(char), 256);

        while (tokens->car->type !=0){
        	Value *token;
        	token = popFromFront(tokens, token);
        	tokens = changeTokens(tokens);
        	tree=addToParseTree(tree, token, &depth); // add to parse tree
            if (depth < 0) {
                SyntaxError();
                depth = 0;
                break;
            }
        	else if ((depth == 0) && (tokens->car->type== 0)) {
                if (define == 1) {
                    Value* val=eval(tree, cell);
                    printValue(val);
                    tree = mallocConsCell(); // reinitialize the tree
                    treeCar = mallocValue();
                    treeCdr = mallocValue();
                    treeCar->type = 0;
                    treeCar->intValue = 0;
                    treeCdr->type = 0;
                    treeCdr->intValue = 0;
                    tree = insertCC(tree, treeCar, treeCdr);
                }
                else{
                    Value* val=eval(tree, cell);
                    if (val->type == 10){
                        define = 1;
                        printValue(val);
                        tree = mallocConsCell(); // reinitialize the tree
                        treeCar = mallocValue();
                        treeCdr = mallocValue();
                        treeCar->type = 0;
                        treeCar->intValue = 0;
                        treeCdr->type = 0;
                        treeCdr->intValue = 0;
                        tree = insertCC(tree, treeCar, treeCdr);
                    }
                    else {
                        printValue(val);
                        tree = mallocConsCell();
                        treeCar = mallocValue();
                        treeCdr = mallocValue();
                        treeCar->type = 0;
                        treeCar->intValue = 0;
                        treeCdr->type = 0;
                        treeCdr->intValue = 0;
                        tree = insertCC(tree, treeCar, treeCdr);
                    }
            	}
            }
            else if ((depth == 0) && (tokens->car->type != 0)){
                SyntaxError();
                depth = 0;
                break;
            }
                
        	}

        cleanupCCLL(headToken);// clean up the tokens list
    }
    if (depth != 0) {
        SyntaxError();
    }   
    free(expression);// free expression    
    freeList(); // free everything...
    return 0;
    }
