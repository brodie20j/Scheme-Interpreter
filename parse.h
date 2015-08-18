#include "escape.h"
#include "parse.c"

Value* popFromFront(ConsCell* tokens, Value* token);

void freeTree(ConsCell* tree);

Value* copyValue(Value* leaf, Value* token);

ConsCell* changeTokens(ConsCell* tokens);

void SyntaxError();

ConsCell* addToParseTree(ConsCell* tree, Value* token, int *depth) ;

void printTree(ConsCell* tree) ;