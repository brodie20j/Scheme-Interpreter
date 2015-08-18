//#ifndef escape
#include "escape.h"
#include "interpret.h"

// given a pointer of something that has been malloc'ed, go to the end of our consCell list and 
// add a conscell that's car value is this pointer. we free this list at the end...
void addToFreeTree(void *pointer){
  ConsCell *movingCons = freeCons;
  while (movingCons!= NULL){
    if (movingCons->cdr->type != 8){
      break;
    }
    movingCons = movingCons->cdr->cons;
  }
  movingCons->car->type = 14;
  movingCons->car->garbageValue = pointer; 
  ConsCell* newCons = malloc(sizeof(ConsCell));
  newCons = insertCC(newCons, createNullVal(), createNullVal());
  movingCons->cdr->type = 8;
  movingCons->cdr->cons = newCons;
}

// function that mallocs a value and puts a pointer of this value in to the
// FreeList...
Value* mallocVal(){
  Value *returnValue = malloc(sizeof(Value));
  void* pointer = returnValue;
  addToFreeTree(pointer);
  return returnValue;
}

// function that mallocs a ConsCell and puts a pointer of this value in to the
// FreeList...
ConsCell* mallocCons(){
  ConsCell* returnCons = malloc(sizeof(ConsCell));
  void* pointer = returnCons;
  addToFreeTree(pointer);
  return returnCons;
}


// looking at a list , pops the first value.
Value* popFromFront(ConsCell* tokens, Value* token){
	token = tokens->car;
	return token;
}

// frees the parsed tree.
void freeTree(ConsCell* tree){
	ConsCell *toGo = tree;
	if (tree->cdr->type == 8){
		tree = tree->cdr->cons;
	
		if (toGo->car->type == 8){
			freeTree(toGo->car->cons);
		}
		else if (toGo->car->type == 4) {
			free(toGo->car->stringValue);
		}
		else if (toGo->car->type == 5) {
			free(toGo->car->symbolValue);
		}
		else if (toGo->car->type == 9) {
			free(toGo->car->idValue);
		}
		
		free(toGo->car);
		free(toGo->cdr);
		free(toGo);
		freeTree(tree);
	}
	else{
		if (toGo->car->type == 8){
			freeTree(toGo->car->cons);
		}
		else if (toGo->car->type == 4) {
			free(toGo->car->stringValue);
		}
		else if (toGo->car->type == 5) {
			free(toGo->car->symbolValue);
		}
		else if (toGo->car->type == 9) {
			free(toGo->car->idValue);
		}
		free(toGo->car);
		free(toGo->cdr);
		free(toGo);

	}
	
}

// given a value, copies it into another already malloc'ed value.
Value* copyValue(Value* leaf, Value* token) {
	leaf->type = token->type;
	char *str_escaped;
	void* pointer;
	switch( leaf->type) {
      case 0:
      break;
      case 1:
      leaf->boolValue = token->boolValue;
      break;
      case 2:
      leaf->intValue = token->intValue;
      break;
      case 3:

      leaf->floatValue = token->floatValue;
      break;
      case 4:
      str_escaped = escape(token->stringValue);
      leaf->stringValue=unescape(str_escaped);
      pointer=leaf->stringValue;
      addToFreeTree(pointer);
      free(str_escaped);
      break;
      case 5:
      str_escaped = escape(token->symbolValue);
      leaf->symbolValue=unescape(str_escaped);
      pointer=leaf->symbolValue;
      addToFreeTree(pointer);
      free(str_escaped);
      break;
      case 6:
      leaf->openValue = token->openValue;
      break;
      case 7:
      leaf->closeValue = token->closeValue;
      break;
      case 8:

      leaf->cons=token->cons;
      break;
      case 9:
      str_escaped = escape(token->idValue);
      leaf->idValue=unescape(str_escaped);
      pointer=leaf->idValue;
      addToFreeTree(pointer);
      free(str_escaped);
      break;
      case 10:
      break;
      case 11:
      leaf->cons= token->cons;
      case 12:
      leaf->clos = token->clos;
      break;
      case 13:
      str_escaped = escape(token->primValue);
      leaf->primValue = unescape(str_escaped);
      pointer=leaf->primValue;
      addToFreeTree(pointer);
      free(str_escaped);
      default:
      break;
    }
    return leaf;

}

// moves the head of the tokens list...
ConsCell* changeTokens(ConsCell* tokens){
	tokens = tokens->cdr->cons;
	return tokens;
}

// called when syntax error is seen.
void SyntaxError() {
	printf("Syntax error.");
}

// function that given a tree, a token, and depth adds to the parse tree.
ConsCell* addToParseTree(ConsCell* tree, Value* token, int *depth) {
	ConsCell *first = tree;
	if (token->type != 7) {
		if (token->type == 6){
			(*depth)++;
		}
		while (tree->cdr->type != 0){ //changes some stuff in parser.c
			tree = tree->cdr->cons;
		}
		ConsCell *newLeaf = mallocCons();
		Value *leafCar = mallocVal();
		Value *leafCdr = mallocVal();
		leafCar = copyValue(leafCar, token);
		leafCdr->type = 0;
		leafCdr->intValue = 0;
		newLeaf = insertCC(newLeaf, leafCar, leafCdr);
		tree->cdr->type=8;
		tree->cdr->cons=newLeaf;
	}
	else {
		(*depth)--;
		if (depth < 0){
			return first;
		}
		ConsCell *last = tree;
		while(tree != NULL){ //changes some stuff in parser.c
			if (tree->car->type == 6){
				last = tree;
			}
			if (tree->cdr->type == 0) {
				break;
			}
			tree = tree->cdr->cons;
		}
		//check if last is first and if last->cdr->type ==0;
		if (last->cdr->type == 0){
			ConsCell *emptyParens = mallocCons();
			Value *emptyCar = mallocVal();
			Value *emptyCdr = mallocVal();
			emptyCar->type= 0;
			emptyCar->intValue = 0;
			emptyCdr->type = 0;
			emptyCdr->intValue = 0;
			emptyParens = insertCC(emptyParens, emptyCar, emptyCdr);
			last->cdr->type = 8;
			last->cdr->cons = emptyParens;
		}
		last->car->type = 8;
		last->car->cons = last->cdr->cons;
		last->cdr->type = 0;
		last->cdr->intValue = 0;
	}
	return first;
}

// prints the Tree.
void printTree(ConsCell* tree) {

	while (tree != NULL) {
	switch( tree->car->type) {
      case 0:
      break;
      case 1:
      printf("%d ", tree->car->boolValue);
      break;
      case 2:
      printf("%i ", tree->car->intValue);
      break;
      case 3:
      printf("%f ", tree->car->floatValue);
      break;
      case 4: 
      printf("'");
      printf("%s", tree->car->stringValue);
      printf("' ");
      break;
      case 5:
      printf("%s ", tree->car->symbolValue);
      break;
      case 6:
      printf("%c ",tree->car->openValue);
      break;
      case 7:
      printf("%c ", tree->car->closeValue);
      break;
      case 8:
      printf("(");
      printTree(tree->car->cons);
      printf(")");
      break;
      case 9:
      printf(" %s", tree->car->idValue);
      break;
      default:
      break;
		}
	if (tree->cdr->type == 0){
		break;
	}
	if (tree->cdr->type == 8){
		tree=tree->cdr->cons;
	}
	else{
		printf(". ");
		switch( tree->cdr->type) {
	      case 0:
	      break;
	      case 1:
	      printf("%d ", tree->cdr->boolValue);
	      break;
	      case 2:
	      printf("%i ", tree->cdr->intValue);
	      break;
	      case 3:
	      printf("%f ", tree->cdr->floatValue);
	      break;
	      case 4: 
	      printf("%s ", tree->cdr->stringValue);
	      break;
	      case 5:
	      printf("%s ", tree->cdr->symbolValue);
	      break;
	      case 6:
	      printf("%c ",tree->cdr->openValue);
	      break;
	      case 7:
	      printf("%c ", tree->cdr->closeValue);
	      break;
	      case 8:
	      printf("(");
	      printTree(tree->cdr->cons);
	      printf(")");
	      break;
	      case 9:
	      printf(" %s", tree->cdr->idValue);
	      break;
	      case 11:
	      printTree(tree->cdr->cons);
	      default:
	      break;
		}
		break;

	}
	}
	
}