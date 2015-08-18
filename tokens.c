//tokens.c
#include <stdbool.h>
#include <string.h>
#include "escape.h"

// for lambda's...
typedef struct __Closure
{
  struct __Value *formal;
  struct __Value *env;
  struct __Value *function;
} Closure;

// for basically everything, we create lists.
typedef struct __ConsCell {
    struct __Value *car;
    struct __Value *cdr;
} ConsCell;

// all the types a value can have...
typedef enum {
  nullType,
  booleanType,
  integerType,
  floatType,
  stringType,
  symbolType,
  openType,
  closeType,
  consType,
  idType,
  voidType,
  quoteType,
  closureType,
  primType, 
  garbageType  
} VALUE_TYPE;


// Values have two things a type and a for lack of a better word a value...
typedef struct __Value {
    VALUE_TYPE type; /* Make sure type includes a consType or some such */
    union {
        bool boolValue;
        int intValue;
        float floatValue;
        char *stringValue;
        struct __ConsCell *cons;
        char *symbolValue;
        char openValue;
        char closeValue;
        char *idValue;
        struct __Closure *clos;
        char *primValue;
        void *garbageValue;
    };
} Value;

// Given a ConsCell, insert the values into its car and cdr and return that conscell.
ConsCell *insertCC(ConsCell *cell, Value *carValue, Value *cdrValue)
{
    cell->car= carValue;
    cell->cdr= cdrValue;
    return cell;
}

//given the head ConsCell, prints out the type and the value. 
void PrintTokens(ConsCell *cell){
  while (cell->car->type != 0){
    switch( cell->car->type) {
      case 0:
      break;
      case 1:
      printf("%d : Boolean\n", cell->car->boolValue);
      break;
      case 2:
      printf("%i : Integer\n", cell->car->intValue);
      break;
      case 3:
      printf("%f : Float\n", cell->car->floatValue);
      break;
      case 4: 
      printf("%s : String\n", cell->car->stringValue);
      break;
      case 5:
      printf("%s : Symbol\n", cell->car->symbolValue);
      break;
      case 6:
      printf("%c : Open\n",cell->car->openValue);
      break;
      case 7:
      printf("%c : Close\n", cell->car->closeValue);
      break;
      case 8:
      break;
      case 9:
      printf("%s : Id\n", cell->car->idValue);
      break;
      case 13:
      printf("%s : Primitive\n", cell->car->primValue);
      break;
      default:
      break;
    }
    cell = cell->cdr->cons;
  }
}


//given the head of the list, cleans up each cell.  IT MAY ALSO NEED TO FREE EACH VALUE!
void cleanupCCLL(ConsCell *cell){
    ConsCell *cur = cell;
    while (cur->car->type != 0)
    {
        ConsCell *toGo = cur;
        if(toGo->car->type==4) free(toGo->car->stringValue);
        else if (toGo->car->type==5) free(toGo->car->symbolValue);
        else if (toGo->car->type==9) free(toGo->car->idValue);
        else if (toGo->car->type==13) free(toGo->car->primValue);

        
        
        cur = cur->cdr->cons;
        free(toGo->car);
        free(toGo->cdr);
        free(toGo);
        }
    
    free(cur->car);
    free(cur);
  }

// when we have a string or symbol trying to figure out where the next token will be.
int getNextTerminal(char *expression, int startingPoint){
  int i;
  for (i= startingPoint; expression[i]; i++){
    if ((expression[i] == ' ') || (expression[i] == ')') || (expression[i] == '\n')){
      return i;
    }  
  } 

  return i;
}


// a function that given our expression and the point we are starting to look at
// returns true if the next set of characters are float values.
int IsFloat(char *expression, int startingPoint) {
  

  int decimalCount=0;
  int numberCount=0;
  if (expression[startingPoint] == '+' || expression[startingPoint] == '-') startingPoint+=1; 
  for (int i=startingPoint; expression[i]; i++) {
    if (expression[i] == '.') {
      decimalCount +=1;
      if (decimalCount>1) {
        return 0;
      }
    }
    else if (expression[i] == ' ') {
      if ((decimalCount == 1) && (numberCount > 0)){
       return 1;
      }
    }
    else if (expression[i] == ')') {
      if ((decimalCount == 1) && (numberCount > 0)){
        return 1;
      }
    }
    else if (expression[i] == '\n') {
      if ((decimalCount == 1) && (numberCount > 0)){
       return 1;
      }
    }
    else if (expression[i] == '1') {
      numberCount += 1;
      continue;
    }
    else if (expression[i] == '2') {
      numberCount += 1;
      continue;
    }
    else if (expression[i] == '3') {
      numberCount += 1;
      continue;
    }
    else if (expression[i] == '4') {
      numberCount += 1;
      continue;
    }
    else if (expression[i] == '5') {
      numberCount += 1;
      continue;
    }
    else if (expression[i] == '6') {
      numberCount += 1;
      continue;
    }
    else if (expression[i] == '7') {
      numberCount += 1;
      continue;
    }
    else if (expression[i] == '8') {
      numberCount += 1;
      continue;
    }
    else if (expression[i] == '9') {
      numberCount += 1;
      continue;
    }
    else if (expression[i] == '0') {
      numberCount += 1;
      continue;
    }
    else {
      return 0;
    }
  }
    return 0;
}

// checks if we are looking at a string.  
int IsString(char *expression, int startingPoint) {
  int quotecount=0;
  if ((expression[startingPoint]=='"') ||(expression[startingPoint]== 39)) {
    quotecount+=1;
    startingPoint++;
  }
  else return 0;
  for (int i=startingPoint; expression[i]; i++) {  
    if ((expression[i]=='"') || (expression[i]== 39))  return 1;
  }
  return 0;
}

// returns a string array given an expression and the starting point
char *returnString(char *expression, int startingPoint) {
  char *str;
  int strlength=0;
  int i=startingPoint+1;
  int j=startingPoint+1;
  while ((expression[i]!='"') && (expression[i]!= 39)) {
    strlength+=1;
    i++;
  }
  str=(char*)malloc((1+strlength)*sizeof(char));
  int k=0;
  while ((expression[j] != '"') && (expression[j] != 39)) {
    str[k]=expression[j];
    j++;
    k++;
  }
  str[k] = '\0';
  char *str_escaped = escape(str);
  free(str);
  return str_escaped;
}

// is the symbol a primitive?
int IsPrimitive(char *symbol){
  if (!strcmp(symbol, "+")){
    return 1;
  }
  else if (!strcmp(symbol, "-")) {
    return 1;
  }
  else if (!strcmp(symbol, "*")) {
    return 1;
  }
  else if (!strcmp(symbol, "/")) {
    return 1;
  }
  else if (!strcmp(symbol, "null?")){
    return 1;
  }
  else if (!strcmp(symbol, "cons")){
    return 1;
  }
  else if (!strcmp(symbol, "car")){
    return 1;
  }
  else if (!strcmp(symbol, "cdr")){
    return 1;
  }
  else if (!strcmp(symbol, "modulo")){
    return 1;
  }
  else if (!strcmp(symbol, "=")) {
    return 1;
  }
  else if (!strcmp(symbol, "<=")) {
    return 1;
  }
  else if (!strcmp(symbol, ">=")) {
    return 1;
  }
  else if (!strcmp(symbol, "<")) {
    return 1;
  }
  else if (!strcmp(symbol, ">")) {
    return 1;
  }
  else if (!strcmp(symbol, "eq?")) {
    return 1;
  }
  else if (!strcmp(symbol, "equal?")) {
    return 1;
  }
  else {
    return 0;
  }
}

// is the symbol a special form type.
int IsId(char *symbol){
  if (!strcmp(symbol, "if")){
    return 1;
  }
  else if (!strcmp(symbol, "and")){
    return 1;
  }
  else if (!strcmp(symbol, "cond")){
    return 1;
  }
  else if (!strcmp(symbol, "begin")){
    return 1;
  }
  else if (!strcmp(symbol, "or")){
    return 1;
  }
  else if (!strcmp(symbol, "let")){
    return 1;
  }
  else if (!strcmp(symbol, "let*")){
    return 1;
  }
  else if (!strcmp(symbol, "letrec")){
    return 1;
  }
  else if (!strcmp(symbol, "quote")){
    return 1;
  }
  else if (!strcmp(symbol, "define")){
    return 1;
  }
  else if (!strcmp(symbol, "lambda")){
    return 1;
  }
  else {
    return 0;
  }
}

// returns true if the set of adjacent characters make up a symbol.
int IsSymbol(char *expression, int startingPoint) {
  int intCount = 0;
  for (int i=0; expression[i]; i++) {
    intCount++;
  }
  int initial=0;
  for (int i=startingPoint; expression[i]; i++) {
    if (expression[i] == '+' || expression[i] == '-') {
      if (expression[i+1]== ' ' || expression[i+1] == '\n' || expression[i+1] == ')' || expression[i+1] == '(') return 1;
      else if (initial>=1) continue;
      else return 0;
    }
    else if((expression[i] >= 'a' && expression[i]<='z') || (expression[i] >= 'A' && expression[i]<='Z') ){
      initial++;
      if (intCount == i+1){
        return 1;
      }

    }
    else if((expression[i] >= '0' && expression[i]<='9') || (expression[i]=='+') || (expression[i] == 45) || (expression[i] == 46)) {
      if (initial>=1) continue;
      else return 0;
    }
    else if (expression[i]== ' ' || expression[i] == '\n' || expression[i] == ')' || expression[i] == '(') {
      if (initial >= 1) {
        return 1;
      }
      else return 0;
    }

    else {
      switch(expression[i]) {
        case 33:
        initial++;
        break;
        case 36:
        initial++;
        break;
        case 37:
        initial++;
        break;
        case 38:
        initial++;
        break;
        case 42:
        initial++;
        break;
        case 47:
        initial++;
        break;
        case 58:
        initial++;
        break;
        case 60:
        initial++;
        break;
        case 61:
        initial++;
        break;
        case 62:
        initial++;
        break;
        case 63:
        initial++;
        break;
        case 94:
        initial++;
        break;
        case 95:
        initial++;
        break;
        case 126:
        initial++;
        break;
        default:
        return 0;
        break;
      }
    }
    

  }
  return 0;
}

// given an expression and starting point, it returns the length of the symbol
int returnSymbolLength(char *expression, int startingPoint) {
  int length=0;
  for (int i=startingPoint; expression[i]; i++) {
    if (expression[i+1]== ' ' || expression[i+1] == '\n' || expression[i+1] == ')' || expression[i+1] == '(') {
      return length;
    }
    length++;
  }
  return length;
}

// returns a symbol based on the length and the starting point in the expression.
char *returnSymbol(char *expression, int startingPoint, int length) {
  char *str;
  str=(char*)calloc(sizeof(char), (3+length));
  int j=startingPoint;
  int i;
  for (i=0; i<=length; i++){
    str[i]=expression[j];
    j++;
  }
  str[i+1]= '\0';
  return str;
}


// from the expression, return the integer we are looking at
int GiveInt(char *expression, int startingPoint) {
  int Integer=0;
  Integer = atoi(&expression[startingPoint]);
  return Integer;
}


// from the expression, return the float we are looking at.
float GiveFloat(char *expression, int startingPoint) {
  float Float=0.0;
  Float = atof(&expression[startingPoint]);
  return Float;
}

// given an expression and starting point, are we looking at an int.
int IsAnInt(char *expression, int startingPoint) {
  int intCount = 0;
  if (expression[startingPoint] == '+' || expression[startingPoint] == '-') startingPoint+=1; 
  for (int i=startingPoint; expression[i]; i++) {
    if (expression[i] == ' '){
      if (intCount >= 1){
        return 1;
      }
      else{
        return 0;
      }
    }
    else if (expression[i] == ')') {
      if (intCount >= 1){
        return 1;
      }
      else{
        return 0;
      }
    }
      
    else if (expression[i] == '\n') {
      if (intCount >= 1){
        return 1;
      }
      else{
        return 0;
      }
    }
    else if (expression[i] =='1') {
      intCount++;
    }
    else if (expression[i] == '2') {
      intCount++;
    }
    else if (expression[i] == '3'){
      intCount++;
    }
    else if (expression[i] == '4') {
      intCount++;
    }
    else if (expression[i] == '5') {
      intCount++;
    }
    else if (expression[i] == '6') {
      intCount++;
    }
    else if (expression[i] == '7') {
      intCount++;
    }
    else if (expression[i] == '8') {
      intCount++;
    }
    else if (expression[i] == '9') {
      intCount++;
    }
    else if (expression[i] == '0') {
      intCount++;
    }
    else return 0;

  }
  return 1;
}


// from the expression and starting point we are at, is it a boolean?
int IsBoolean(char *expression, int startingPoint) {
  for (int i=startingPoint; expression[i]; i++) {
    if (expression[i] == '#') {
      if (expression[i+1] == 't' || expression[i+1] == 'f') return 1;
    }
    return 0;
  }
  return 0;
}


// return the boolean we are looking at from the expression and the starting point.
bool returnBoolean(char *expression, int startingPoint) {
  bool boolean = false;    
  if (expression[startingPoint+1] == 't') {
    boolean = true;
  }
  else {
    boolean = false;
  }
  return boolean;
}

// finds the length of the string array.
int findLengthOfString(char *string) {
  int i = 1;
  for (int l = 0; string[l]; l++) {
    i++;
  }
  return i;
}

// function that tokenizes the input, using a structure of conscell's, where each conscell has a car and a cdr,
// each car has two values, the type and the actual value.
ConsCell* tokenize(char *expression) {
  ConsCell *current =malloc(sizeof(ConsCell));
  ConsCell *Head = current;
  int i = 0;
  for (int i = 0; expression[i]; i++) {
    if (expression[i]==' ') continue;
      else if (expression[i] == '\n'){
        continue;
      }
    if (expression[i]== '\t'){
      continue;
    }
    Value *carVar = malloc(sizeof(Value));
    Value *cdrVar = malloc(sizeof(Value));
    if (expression[i]=='(') {
      carVar->type = 6;
      carVar->openValue = '(';
    }

    else if (expression[i]==')') {
      carVar->type=7;
      carVar->closeValue = ')';
    }
    else {
      
      if (IsAnInt(expression, i)) {
        carVar->type=2;
        carVar->intValue=GiveInt(expression, i);
        i = getNextTerminal(expression, i);
        i -=1;
      }
      else if(IsSymbol(expression, i)) {
        int symbolLength=returnSymbolLength(expression, i);
        carVar->type=5;
        carVar->symbolValue=returnSymbol(expression, i, symbolLength);
        if (IsId(carVar->symbolValue)){
          carVar->type = 9;
          carVar->idValue = carVar->symbolValue;
        }
        else if (IsPrimitive(carVar->symbolValue)) {
          carVar->type = 13;
          carVar->primValue = carVar->symbolValue;
        }
        i+=symbolLength;
      }
      else if(IsString(expression, i)) {
        carVar->type=4;
        carVar->stringValue=returnString(expression, i);
        int lengthOfString = findLengthOfString(carVar->stringValue);
        i+=lengthOfString;
      }
      else if(IsBoolean(expression,i)){
        carVar->type = 1;
        carVar->boolValue=returnBoolean(expression,i);
        i +=1;
      }
      else if(IsFloat(expression, i)) {
        carVar->type=3;
        carVar->floatValue=GiveFloat(expression, i);
        i = getNextTerminal(expression, i);
        i -=1;
      }
      else if(expression[i] == ';') {
        free(carVar);
        free(cdrVar);
        break;
      }
      else {
        free(carVar);
        free(cdrVar);
        printf("Error: Bad syntax.\n");
        ConsCell *emptyCons =malloc(sizeof(ConsCell));
        Value *Val = malloc(sizeof(Value));
        Val->type = 0;
        Val->intValue= 1;
        emptyCons->car = Val;
        Value *cdrVal = malloc(sizeof(Value));
        cdrVal->type = 0;
        cdrVal->intValue = 0;
        current->car = cdrVal;
        cleanupCCLL(Head);
        return emptyCons;
        break;
      }
      
      


    }
    ConsCell *newCell = malloc(sizeof(ConsCell));
    cdrVar->type = 8;
    cdrVar->cons = newCell;
    current = insertCC(current, carVar, cdrVar);
    current = newCell;

  }
  Value *cdrVal = malloc(sizeof(Value));
  cdrVal->type = 0;
  cdrVal->intValue = 0;
  current->car = cdrVal;
  return Head;

}



