//tokens.h
#include <stdbool.h>
#include <string.h>
#include "escape.h"
#include "tokens.c"

// Given a ConsCell, insert the values into its car and cdr and return that conscell.
ConsCell *insertCC(ConsCell *cell, Value *carValue, Value *cdrValue);

//given the head ConsCell, prints out the type and the value. 
void PrintTokens(ConsCell *cell);

int IsId(char *symbol);

int IsPrimitive(char *symbol);

//given the head of the list, cleans up each cell.  IT MAY ALSO NEED TO FREE EACH VALUE!
void cleanupCCLL(ConsCell *cell);

int getNextTerminal(char *expression, int startingPoint);

// a function that given our expression and the point we are starting to look at
// returns true if the next set of characters are float values.
int IsFloat(char *expression, int startingPoint) ;

// checks if we are looking at a string.  
int IsString(char *expression, int startingPoint) ;

// returns a string array given an expression and the starting point
char *returnString(char *expression, int startingPoint) ;

// returns true if the set of adjacent characters make up a symbol.
int IsSymbol(char *expression, int startingPoint);
// given an expression and starting point, it returns the length of the symbol

int returnSymbolLength(char *expression, int startingPoint);

// returns a symbol based on the length and the starting point in the expression.
char *returnSymbol(char *expression, int startingPoint, int length);


// from the expression, return the integer we are looking at
int GiveInt(char *expression, int startingPoint);

// from the expression, return the float we are looking at.
float GiveFloat(char *expression, int startingPoint);

// given an expression and starting point, are we looking at an int.
int IsAnInt(char *expression, int startingPoint); 


// from the expression and starting point we are at, is it a boolean?
int IsBoolean(char *expression, int startingPoint);


// return the boolean we are looking at from the expression and the starting point.
bool returnBoolean(char *expression, int startingPoint);

// finds the length of the string array.
int findLengthOfString(char *string);

// function that tokenizes the input, using a structure of conscell's, where each conscell has a car and a cdr,
// each car has two values, the type and the actual value.
ConsCell* tokenize(char *expression);

ConsCell* freeCons;


