//interpret.c
#include <stdbool.h>
#include <string.h>
#include "tokens.h"
#include "parse.h"
#include <stdio.h>
#include "interpret.h"


// function that creates a null Value...
Value* createNullVal() {
  Value* tempValue = malloc(sizeof(Value));
  tempValue->type = 0;
  tempValue->intValue = 0;
  return tempValue;
}

// given an already malloc'ed value, turns it into a null value...
Value* setNullVal(Value* mlcdVal){
  mlcdVal->type = 0;
  mlcdVal->intValue = 0;
  return mlcdVal;
}

// creates a new value that will be freed in the end.  We switched to this for freeing purposes.
Value* newCreateNullVal(){
  Value* tempValue = mallocValue();
  tempValue = setNullVal(tempValue);
  return tempValue;
}

// given a pointer of something that has been malloc'ed, go to the end of our consCell list and 
// add a conscell that's car value is this pointer. we free this list at the end...
void addToFreeList(void *pointer){
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
Value* mallocValue(){
  Value *returnValue = malloc(sizeof(Value));
  void* pointer = returnValue;
  addToFreeList(pointer);
  return returnValue;
}

// function that mallocs a Closure and puts a pointer of this value in to the
// FreeList...
Closure* mallocClosure(){
  Closure *returnValue = malloc(sizeof(Closure));
  void* pointer = returnValue;
  addToFreeList(pointer);
  return returnValue;
}

// function that mallocs a ConsCell and puts a pointer of this value in to the
// FreeList...
ConsCell* mallocConsCell(){
  ConsCell* returnCons = malloc(sizeof(ConsCell));
  void* pointer = returnCons;
  addToFreeList(pointer);
  return returnCons;
}

// function that given the global head of the freeList, frees the pointers in each consCell and
// the list itself...
void freeList(){
  while (freeCons != NULL){
    if (freeCons->cdr->type == 8){
      ConsCell* ToGo = freeCons;
      freeCons = freeCons->cdr->cons;
      free(ToGo->car->garbageValue);
      free(ToGo->car);
      free(ToGo->cdr);
      free(ToGo);
    }
    else {
      free(freeCons->car);
      free(freeCons->cdr);
      free(freeCons);
      break;
    }
  }
}

// function that just creates an environment, this is called at the beginning of interpreter.c
// and times like when let is called...
ConsCell* createEnvironment() {
    ConsCell *parent = mallocConsCell();
    Value *pcarVar = mallocValue();
    Value *pcdrVar = mallocValue();
    pcarVar->type=8;
    pcdrVar->type=0;
    parent=insertCC(parent, pcarVar, pcdrVar);
    ConsCell *cell = mallocConsCell();
    Value *carVar = mallocValue();
    Value *cdrVar = mallocValue();
    carVar->type=0;
    cdrVar->type=0;
    parent->car->cons=insertCC(cell,carVar, cdrVar);
    return parent;
}

// Given an environment and two values, it inserts a frame into an environment.
// the frame's car points to a cons cell where the car is a symbol and the cdr is a binding...
ConsCell* insertFrame(ConsCell *environment, Value *symbol, Value *binding) {
  ConsCell *frame = environment->car->cons;
  while (frame->car->type != 0){
    frame = frame->cdr->cons;
  }
  ConsCell *cell = mallocConsCell();
  cell = insertCC(cell, symbol, binding);
  frame->car->type = 8;
  frame->car->cons = cell;
  frame->cdr->type = 8;
  ConsCell *emptyCell = mallocConsCell();
  Value *ecarVar = mallocValue();
  Value *ecdrVar = mallocValue();
  ecarVar->type=0;
  ecdrVar->type=0;
  frame->cdr->cons = insertCC(emptyCell, ecarVar, ecdrVar);
  return environment;
}

// Creates a child environment that points back to a previous environment.
ConsCell* insertEnvironment(ConsCell *parentEnvironment){
  ConsCell* newEnvironment = createEnvironment();
  newEnvironment->cdr->type = 8;
  newEnvironment->cdr->cons = parentEnvironment;
  return newEnvironment;
}

// With an environment, and hopefully a symbol, returns a value if it is in the environment. 
Value* resolveVariable(Value *val, ConsCell *environment) {
  ConsCell* frame = environment->car->cons;
  Value* tempValue;
  while (frame->car->type != 0){
    if (!strcmp(frame->car->cons->car->symbolValue, val->symbolValue)){
      tempValue = mallocValue();
      tempValue = copyValue(tempValue, frame->car->cons->cdr);
      return tempValue;
    }
    frame = frame->cdr->cons;
  }
  if (environment->cdr->type == 8){
    return resolveVariable(val, environment->cdr->cons);
  }
  return newCreateNullVal();
}

// we make an environment and input all bindings and evaluate a body in that environment...
Value* evalLet(ConsCell *varBinding, ConsCell *body, ConsCell *env) {
  ConsCell* newEnvironment=insertEnvironment(env);
  Value* temporaryValue;
  while(varBinding->car->type != 0) {
    if (varBinding->car->type == 8){
      //errorchecking...
      temporaryValue = mallocValue();
      temporaryValue = copyValue(temporaryValue, varBinding->car->cons->car);
      insertFrame(newEnvironment, temporaryValue, eval(varBinding->car->cons->cdr->cons, env));
    }
    if (varBinding->cdr->type != 8) {
      break;
    }
    varBinding = varBinding->cdr->cons;
  }
  Value* returnValue = eval(body, newEnvironment);
  return returnValue;
}



//We did letrec here...  We keep adding to a single frame using this new environment.
//The only line that is different from let is (insertFrame(newEnvironment...)).
Value* evalLetStar(ConsCell *varBinding, ConsCell *body, ConsCell *env) {
  ConsCell* newEnvironment=insertEnvironment(env);
  Value* temporaryValue;
  while(varBinding->car->type != 0) {
    if (varBinding->car->type == 8){
      temporaryValue = mallocValue();
      temporaryValue = copyValue(temporaryValue, varBinding->car->cons->car);
      insertFrame(newEnvironment, temporaryValue, eval(varBinding->car->cons->cdr->cons, newEnvironment));
    }
    if (varBinding->cdr->type != 8) {
      break;
    }
    varBinding = varBinding->cdr->cons;
  }
  Value* returnValue = eval(body, newEnvironment);
  return returnValue;
}


// function that evals quote, the value returned is its own value that has a cons that points to the tree.
Value* evalQuote(ConsCell *start){
  Value *quoteValue = mallocValue();
  quoteValue->type =11;
  quoteValue->cons = start;
  return quoteValue;
}

// function that cleans up environment. Used no longer...
// void cleanupEnvironment(ConsCell* parentEnvironment) {
//   ConsCell *toGo=parentEnvironment;
//   parentEnvironment = parentEnvironment->car->cons;
//   free(toGo->car);
//   free(toGo->cdr);
//   free(toGo);

//   while (parentEnvironment->car->type != 0) {
//     if (parentEnvironment->car->type == 8){
//       //we have a child environment

//       if (parentEnvironment->car->cons->car->type == 5){
//         free(parentEnvironment->car->cons->car->symbolValue);
//         free(parentEnvironment->car->cons->car);
//         if (parentEnvironment->car->cons->cdr->type == 4){
//           free(parentEnvironment->car->cons->cdr->stringValue);
//         }
//         free(parentEnvironment->car->cons->cdr);
//         free(parentEnvironment->car->cons);
//       }
//       else {
//       free(parentEnvironment->car->cons->car);
//       free(parentEnvironment->car->cons->cdr);
//       free(parentEnvironment->car->cons);
//       }
//     }
//     toGo = parentEnvironment;
//     parentEnvironment = parentEnvironment->cdr->cons;
//     free(toGo->car);
//     free(toGo->cdr);
//     free(toGo);
//   }
//   free(parentEnvironment->car);
//   free(parentEnvironment->cdr);
//   free(parentEnvironment);
// }


// function that prints values...
void printValue(Value *evaluation){
  switch (evaluation->type)  {
    case 0:
    printf("Evaluation Error\n");
    break;
    case 1:
    if (evaluation->boolValue == 1){
      printf("#t\n");
    }
    else {
      printf("#f\n");
    }
    break;
    case 2:
    printf("%i\n", evaluation->intValue);
    break;
    case 3:
    printf("%f\n", evaluation->floatValue);
    break;
    case 4:
    printf("'");
    printf("%s", evaluation->stringValue);
    printf("'\n");
    break;
    case 5:
    printf("%s\n", evaluation->symbolValue);
    break;
    case 6:
    printf("%c\n", evaluation->openValue);
    break;
    case 7:
    printf("%c\n", evaluation->closeValue);
    break;
    case 8:
    printTree(evaluation->cons);
    break;
    break;
    case 9:
    break;
    case 10:
    break;
    case 11:
    printf("'");
    printTree(evaluation->cons);
    printf("\n");
    default:
    break;
  }
}

// function that takes in (if *boolean *first *second) and also *environment, will return a value.
Value* evalIf(ConsCell *boolean, ConsCell *first, ConsCell *second, ConsCell *env) {
  Value* tempValue;
  Value* myValue;
  if (boolean->car->type == 1){
    if (boolean->car->boolValue == 1){
      if ((first->car->type == 1) || (first->car->type == 2) || (first->car->type == 3) || (first->car->type == 4) ||(first->car->type == 5) ||(first->car->type == 8)){
          return eval(first, env);
      }
    }
    else{
      if ((second->car->type == 1) || (second->car->type == 2) || (second->car->type == 3) || (second->car->type == 4) ||(second->car->type == 5)||(second->car->type == 8)){
          return eval(second, env);
        } 
    }
  }
  else if (boolean->car->type == 5){
    myValue = resolveVariable(boolean->car, env);
    if (myValue->type == 1) {
      if(myValue->boolValue == 1){
        free(myValue);
        if ((first->car->type == 1) || (first->car->type == 2) || (first->car->type == 3) || (first->car->type == 4) ||(first->car->type == 5) ||(first->car->type == 8)){
          return eval(first, env);
        } 
      }
      else if(myValue->boolValue == 0){
        free(myValue);
        if ((second->car->type == 1) || (second->car->type == 2) || (second->car->type == 3) || (second->car->type == 4) ||(second->car->type == 5)||(second->car->type == 8)){
          return eval(second, env);
        } 
      }
    }
    else if(myValue->type == 11){
      ConsCell* newBoolean = myValue->cons;
      free(myValue);
      return evalIf(newBoolean, first, second, env);
    }
    else {
      free(myValue);
      return newCreateNullVal();
    }
  } else if (boolean->car->type == 9) {
      tempValue=eval(boolean, env);
      if (tempValue->type != 1) {
        return newCreateNullVal(); 
      }
      if (eval(boolean, env)->boolValue == 1) {
        return eval(first, env);
      } else {
        return eval(second, env);
      }
    }
    else if (boolean->car->type == 8) {
      Value *tempValue = eval(boolean, env);
      if (tempValue->boolValue) {
        return eval(first, env);
      }
      else {
        return eval(second, env);
      }
    }
  else {
    return newCreateNullVal();
  } 
  return newCreateNullVal();
  } 
  
// var is a in (define a 3), expr is 3. 
Value* evalDefine(ConsCell *var, ConsCell *expr, ConsCell *env){
  Value *variable = mallocValue();
  variable = copyValue(variable, var->car);
  Value *templeValue = eval(expr, env);
  insertFrame(env, variable, templeValue);
  Value* tempValue;
  tempValue = mallocValue();
  tempValue->type = 10;
  tempValue->intValue = 0;
  return tempValue;
}

// this is for defining functions, we create a closure and return a value
// that points to the closure... (A closure is like a conscell that has 3 values...)
Value* evalLambda(ConsCell *params, ConsCell *body, ConsCell *env){
  Closure *lambdaClosure = mallocClosure();
  Value *formalList = mallocValue();
  Value *funcBody = mallocValue();
  Value *envPtr = mallocValue();
  formalList->type = 8;
  funcBody->type = 8;
  envPtr->type = 8;
  formalList->cons = params;
  funcBody->cons = body;
  envPtr->cons = env;
  lambdaClosure->formal = formalList;
  lambdaClosure->function = funcBody;
  lambdaClosure->env = envPtr;
  Value *returnValue = mallocValue();
  returnValue->type = 12;
  returnValue->clos = lambdaClosure;
  return returnValue;
}

// given a closure, binds formal and actual together and then evaluates the function
// body in the environment where we were just binding...
Value* applyFunc(Value* closure, ConsCell* expression, ConsCell *env){
  Closure *lambdaClosure = closure->clos;
  ConsCell *newEnvironment = insertEnvironment(lambdaClosure->env->cons);
  ConsCell *formalList = lambdaClosure->formal->cons;
  // this if is all about the binding...
  if ((formalList->car->cons->car->type != 0) && (expression->cdr->type == 8)) {
    formalList = formalList->car->cons;
    Value* tempValue;
    expression = expression->cdr->cons;
    while (expression != NULL){
      tempValue = mallocValue();
      tempValue = copyValue(tempValue, formalList->car);
      if (tempValue->type == 0){
        return tempValue;
      }
      insertFrame(newEnvironment, tempValue , eval(expression, env));
      if ((expression->cdr->type != 8) && (formalList->cdr->type != 8)){
        return eval(lambdaClosure->function->cons, newEnvironment);
      }
      else if(expression->cdr->type != 8) {
        return newCreateNullVal();
      }
      else if (formalList->cdr->type != 8){
        return newCreateNullVal();
      }
      formalList = formalList->cdr->cons;
      expression = expression->cdr->cons;
    }
  }
  if (expression->cdr->type == 8){
    return newCreateNullVal();
  }
  if (formalList->car->type == 8){
    if (formalList->car->cons->car->type != 0){
      return newCreateNullVal();
    }
  }
  // eval function body...
  return eval(lambdaClosure->function->cons, newEnvironment);
}

// primitive function: null?
// given an expression returns true if expression is '()
// otherwise returns false.
Value* evalNull(Value *expression, ConsCell* env){
  Value *tempValue;
  if (expression->type != 11){
    tempValue = mallocValue();
    tempValue->type = 1;
    tempValue->boolValue = 0;
    return tempValue;
  }
  if (expression->cons->car->cons->car->type != 0){
    tempValue = mallocValue();
    tempValue->type = 1;
    tempValue->boolValue = 0;
    return tempValue;
  }
  else{
    tempValue = mallocValue();
    tempValue->type = 1;
    tempValue->boolValue = 1;
    return tempValue;
  }
  return newCreateNullVal();
}

// primitive function: car
// returns the first thing from a list or pair...
Value *evalCar(Value *expr, ConsCell* env){
  if (expr->type != 11){
    return newCreateNullVal();
  }
  else{
    Value *returnValue = mallocValue();
    returnValue->type = 11;
    ConsCell *containerCons = mallocConsCell();
    returnValue->cons = containerCons;
    Value *tempValue = mallocValue();
    tempValue = copyValue(tempValue, expr->cons->car->cons->car);
    insertCC(containerCons, tempValue, newCreateNullVal());
    return returnValue;
  }
}

// primitive function: cdr
// returns a list without the first item if it is a list.
// returns the second item if it is a pair.
Value *evalCdr(Value *expr, ConsCell* env){
  if (expr->type != 11){
    return newCreateNullVal();
  }
  else if ((expr->cons->car->type == 8) && (expr->cons->car->cons->car->type == 0)){
    return newCreateNullVal();
  }
  else{
    if (expr->cons->car->type == 8){
      if ((expr->cons->car->cons->cdr->type < 6) && (expr->cons->car->cons->cdr->type > 0)) {
        Value *tempValue = newCreateNullVal();
        tempValue = copyValue(tempValue, expr->cons->car->cons->cdr);
        return tempValue;
      }
    }
    //return the list without the first item in it.
    Value *returnValue = mallocValue();
    returnValue->type = 11;
    ConsCell *quoteCons = mallocConsCell();
    returnValue->cons = quoteCons;
    Value *quoteCar = mallocValue();
    Value *quoteCdr = newCreateNullVal();
    quoteCar->type = 8;
    if (expr->cons->car->cons->cdr->type == 8){
      quoteCar->cons = expr->cons->car->cons->cdr->cons;
    }
    else {
      ConsCell *tempCons = mallocConsCell();
      quoteCar->cons = insertCC(tempCons, newCreateNullVal(), newCreateNullVal());
    }
    quoteCons = insertCC(quoteCons, quoteCar, quoteCdr);
    return returnValue;
  }
}

// primitive function: Cons
// given two values, returns the (cons first second) of them.
Value *evalCons(Value *first, Value *second, ConsCell* env) {
  Value *returnValue = mallocValue();
  returnValue->type = 11;
  ConsCell *quoteCons = mallocConsCell();
  returnValue->cons = quoteCons;
  ConsCell* containerCons = mallocConsCell();
  if (first->type == 11){
    containerCons->car = first->cons->car;
  }
  else {
    containerCons->car = first;
  }
  if (second->type == 11){
    if (second->cons->car->type == 8){
      Value *cdr = mallocValue();
      cdr->type = 8;
      cdr->cons = second->cons->car->cons; 
      containerCons->cdr = cdr;
    }
  else {
    containerCons->cdr = second;
    }
  }
  else {
    containerCons->cdr = second;
    }
  Value *quoteCar = mallocValue();
  Value *quoteCdr = newCreateNullVal();
  quoteCar->type = 8;
  quoteCar->cons = containerCons;
  insertCC(quoteCons, quoteCar, quoteCdr);
  return returnValue;  
}
  /*
Value *evalList(ConsCell *cur, ConsCell* env) {
  Value *returnValue = mallocValue();
  returnValue->type = 11;
  ConsCell *quoteCons = mallocConsCell();
  returnValue->cons = quoteCons;
  ConsCell* containerCons = mallocConsCell();
  ConsCell* cur=first;
  while (cur->cdr->type != 0) {
    ConsCell* cell=mallocConsCell();
    Value *val1=mallocValue();
    Value *cdr=mallocValue();
    val1=copyValue(first->car);
    cdr->type=8;


  }

  Value *value1=mallocValue();
  Value *value2=mallocValue();
  value1=copyValue(value1, first);
  value2=copyValue(value2, second);
    containerCons->car = value1;
      Value *cdr = mallocValue();
      cdr->type = value2->type;
      cdr=value2;
      containerCons->cdr = cdr;
  Value *quoteCar = mallocValue();
  Value *quoteCdr = newCreateNullVal();
  quoteCar->type = 8;
  quoteCar->cons = containerCons;
  insertCC(quoteCons, quoteCar, quoteCdr);
  return returnValue;  
}*/


// primitive function: Subtraction
// Given an expression, evalSubtraction subtracts the second part of the body of the expression from the first
Value* evalSubtraction(ConsCell *expr, ConsCell* env){
  Value *tempValue;
  Value *varVal;
  expr= expr->cdr->cons;
  tempValue = mallocValue();
  tempValue->type = 0;
  tempValue->intValue = 0;

if (expr->car->type == 2){
    tempValue->type=2;
    tempValue->intValue=expr->car->intValue;
  }
else if (expr->car->type == 3){
    tempValue->type=3;
    tempValue->floatValue=expr->car->floatValue;
    }

  else if ((expr->car->type == 8) || (expr->car->type == 5) || (expr->car->type == 11)){
      varVal = eval(expr, env);
      if (varVal->type == 2){
          tempValue->type=2;
          tempValue->intValue=varVal->intValue;
      }
      else if (varVal->type == 3){
          tempValue->type = 3;
          tempValue->floatValue = varVal->floatValue;
        }
      else {
        return newCreateNullVal();
      }
    }
    else {
      return newCreateNullVal();
    }
    expr = expr->cdr->cons;
  while (expr!= NULL){
    if (expr->car->type == 2){
      if (tempValue->type == 2){
        tempValue->intValue = tempValue->intValue - expr->car->intValue;
      }
      else{
        tempValue->floatValue = tempValue->floatValue - expr->car->intValue;
      }
    }
    else if (expr->car->type == 3){
      if (tempValue->type == 2){
        tempValue->type = 3;
        tempValue->floatValue = tempValue->intValue;
      }
      tempValue->floatValue = tempValue->floatValue - expr->car->floatValue;
    }

    else if ((expr->car->type == 8) || (expr->car->type == 5) || (expr->car->type == 11)){
      varVal = eval(expr, env);
      if (varVal->type == 2){
        if (tempValue->type == 2){
          tempValue->intValue = tempValue->intValue - varVal->intValue;
        }
        else{
          tempValue->floatValue = tempValue->floatValue - varVal->intValue;
        }
      }
      else if (varVal->type == 3){
        if (tempValue->type == 2){
          tempValue->type = 3;
          tempValue->floatValue = tempValue->intValue;
        }
        tempValue->floatValue = tempValue->floatValue - varVal->floatValue;
      }
      else {
        return newCreateNullVal();
      }
    }
    else {
      return newCreateNullVal();
    }
    if (expr->cdr->type != 8){
      return tempValue;
    }
    expr = expr->cdr->cons;
  }
  return tempValue;
}
// primitive function: Modulo
//assuming its two arguments are ints, return the modulo of the two arguments
Value* evalModulo(ConsCell *expr, ConsCell* env) {
  Value *tempValue;
  Value *varVal1;
  Value *varVal2;
  expr=expr->cdr->cons;
  varVal1=eval(expr, env);
  varVal2=eval(expr->cdr->cons, env);

  if ((varVal1->type != 2) || (varVal2->type != 2)) return newCreateNullVal();
  else {
    tempValue=mallocValue();
    tempValue->type=2;
    tempValue->intValue= varVal1->intValue % varVal2->intValue;
    return tempValue;
  }
}

// primitive function: =
//checks if both numbers are equal. If anything is supplied other than an int or float an error is passed
Value *evalEquals(ConsCell *expr, ConsCell* env) {
  Value *tempValue;
  Value *varVal1;
  Value *varVal2;
  expr=expr->cdr->cons;

  varVal1=eval(expr, env);
  varVal2=eval(expr->cdr->cons, env);
  
  if (((varVal1->type != 2) && (varVal1->type != 3)) || ((varVal2->type != 2) && (varVal2->type != 3))) {
    return newCreateNullVal();
  }
  else {
    tempValue=mallocValue();
    tempValue->type=1;
    if ((varVal1->type==3) && (varVal2->type==2)) {
      if (varVal1->floatValue == varVal2->intValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    else if ((varVal1->type==2) && (varVal2->type==3)) {
      if (varVal1->intValue == varVal2->floatValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    else if ((varVal1->type==3) && (varVal2->type==3)) {
      if (varVal1->floatValue == varVal2->floatValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    else if ((varVal1->type==2) && (varVal2->type==2)) {
      if (varVal1->intValue == varVal2->intValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    return tempValue;
  }
    return newCreateNullVal();
}



// primitive function: >
//checks if first argument is greater than the second argument. If anything is supplied other than an int or float an error is passed.
Value *evalGreaterThan(ConsCell *expr, ConsCell* env) {
  Value *tempValue;
  Value *varVal1;
  Value *varVal2;
  expr=expr->cdr->cons;
  varVal1=eval(expr, env);
  varVal2=eval(expr->cdr->cons, env);
  if (((varVal1->type != 2) && (varVal1->type != 3)) || ((varVal2->type != 2) && (varVal2->type != 3))) {
    return newCreateNullVal();
  }
  else {
    tempValue=mallocValue();
    tempValue->type=1;
    if ((varVal1->type==3) && (varVal2->type==2)) {
      if (varVal1->floatValue > varVal2->intValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    else if ((varVal1->type==2) && (varVal2->type==3)) {
      if (varVal1->intValue > varVal2->floatValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    else if ((varVal1->type==3) && (varVal2->type==3)) {
      if (varVal1->floatValue > varVal2->floatValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    else if ((varVal1->type==2) && (varVal2->type==2)) {
      if (varVal1->intValue > varVal2->intValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    return tempValue;
  }
    return newCreateNullVal();
}

// primitive function: <
//checks if first argument is less than the second argument. If anything is supplied other than an int or float an error is passed.
Value *evalLessThan(ConsCell *expr, ConsCell* env) {
  Value *tempValue;
  Value *varVal1;
  Value *varVal2;
  expr=expr->cdr->cons;

  varVal1=eval(expr, env);
  varVal2=eval(expr->cdr->cons, env);
  if (((varVal1->type != 2) && (varVal1->type != 3)) || ((varVal2->type != 2) && (varVal2->type != 3))) {
    return newCreateNullVal();
  }
  else {
    tempValue=mallocValue();
    tempValue->type=1;
    if ((varVal1->type==3) && (varVal2->type==2)) {
      if (varVal1->floatValue < varVal2->intValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    else if ((varVal1->type==2) && (varVal2->type==3)) {
      if (varVal1->intValue < varVal2->floatValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    else if ((varVal1->type==3) && (varVal2->type==3)) {
      if (varVal1->floatValue < varVal2->floatValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    else if ((varVal1->type==2) && (varVal2->type==2)) {
      if (varVal1->intValue < varVal2->intValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    return tempValue;
  }
    return newCreateNullVal();
}

// primitive function: >=
//checks if first argument is greater than or equal to the second argument. If anything is supplied other than an int or float an error is passed.
Value *evalGreaterThanEqualTo(ConsCell *expr, ConsCell* env) {
  Value *tempValue;
  Value *varVal1;
  Value *varVal2;
  expr=expr->cdr->cons;
  varVal1=eval(expr, env);
  varVal2=eval(expr->cdr->cons, env);
  if (((varVal1->type != 2) && (varVal1->type != 3)) || ((varVal2->type != 2) && (varVal2->type != 3))) {
    return newCreateNullVal();
  }
  else {
    tempValue=mallocValue();
    tempValue->type=1;
    if ((varVal1->type==3) && (varVal2->type==2)) {
      if (varVal1->floatValue >= varVal2->intValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    else if ((varVal1->type==2) && (varVal2->type==3)) {
      if (varVal1->intValue >= varVal2->floatValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    else if ((varVal1->type==3) && (varVal2->type==3)) {
      if (varVal1->floatValue >= varVal2->floatValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    else if ((varVal1->type==2) && (varVal2->type==2)) {
      if (varVal1->intValue >= varVal2->intValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    return tempValue;
  }
    return newCreateNullVal();
}

// primitive function: <=
//checks if first argument is less than or equal to the second argument. If anything is supplied other than an int or float an error is passed.
Value *evalLessThanEqualTo(ConsCell *expr, ConsCell* env) {
  Value *tempValue;
  Value *varVal1;
  Value *varVal2;
  expr=expr->cdr->cons;

  varVal1=eval(expr, env);
  varVal2=eval(expr->cdr->cons, env);
  if (((varVal1->type != 2) && (varVal1->type != 3)) || ((varVal2->type != 2) && (varVal2->type != 3))) {
    return newCreateNullVal();
  }
  else {
    tempValue=mallocValue();
    tempValue->type=1;
    if ((varVal1->type==3) && (varVal2->type==2)) {
      if (varVal1->floatValue <= varVal2->intValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    else if ((varVal1->type==2) && (varVal2->type==3)) {
      if (varVal1->intValue <= varVal2->floatValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    else if ((varVal1->type==3) && (varVal2->type==3)) {
      if (varVal1->floatValue <= varVal2->floatValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    else if ((varVal1->type==2) && (varVal2->type==2)) {
      if (varVal1->intValue <= varVal2->intValue) {
        tempValue->boolValue=1;
      } else tempValue->boolValue=0;
    }
    return tempValue;
  }
    return newCreateNullVal();
}

// function that evaluates an or expression.
Value* evalOr(ConsCell *expr, ConsCell* env) {
  Value *tempValue;
  Value *evalBool;

  //if there are no arguments, return false.
  if (expr->cdr->cons == NULL) {
    tempValue=mallocValue();
    tempValue->type=1;
    tempValue->boolValue=0;
    return tempValue;
  }
  //evaluate each of these until we either get to the end or encounter a true case, in which case return.
  do {
    expr=expr->cdr->cons;
    evalBool=eval(expr, env);
    if (evalBool->type == 1) {
      if (evalBool->boolValue==1) {
        return evalBool;
      }
    }

    } while (expr->cdr->type != 0);
    return evalBool;
}

// function that evaluates an and expression.
Value* evalAnd(ConsCell *expr, ConsCell* env) {
  Value *tempValue;
  Value *evalBool;

  //if there are no arguments, return true.
  if (expr->cdr->cons==NULL) {
    tempValue=mallocValue();
    tempValue->type=1;
    tempValue->boolValue=1;
    return tempValue;
  }
  //evaluate each of these until we either get to the end or encounter a false case, in which case return.
  do {
    expr=expr->cdr->cons;
    evalBool=eval(expr, env);
    if (evalBool->type == 1) {
      if (evalBool->boolValue == 0) {
        return evalBool;
        }
      }
    } while (expr->cdr->type != 0);

    return evalBool;
}

// function that evaluates a + expression.
Value* evalAddition(ConsCell *expr, ConsCell* env){
  Value *tempValue;
  Value *varVal;
  
  if (expr->cdr->type != 8){
    tempValue = mallocValue();
    tempValue->type = 2;
    tempValue->intValue = 0;
    return tempValue;
  }
  expr= expr->cdr->cons;
  tempValue = mallocValue();
  tempValue->type = 2;
  tempValue->intValue = 0;
  while (expr!= NULL){
    if (expr->car->type == 2){
      if (tempValue->type == 2){
        tempValue->intValue = tempValue->intValue + expr->car->intValue;
      }
      else{
        tempValue->floatValue = tempValue->floatValue + expr->car->intValue;
      }
    }
    else if (expr->car->type == 3){
      if (tempValue->type == 2){
        tempValue->type = 3;
        tempValue->floatValue = tempValue->intValue;
      }
      tempValue->floatValue = tempValue->floatValue + expr->car->floatValue;
    }

    else if ((expr->car->type == 8) || (expr->car->type == 5) || (expr->car->type == 11)){
      varVal = eval(expr, env);
      if (varVal->type == 2){
        if (tempValue->type == 2){
          tempValue->intValue = tempValue->intValue + varVal->intValue;
        }
        else{
          tempValue->floatValue = tempValue->floatValue + varVal->intValue;
        }
      }
      else if (varVal->type == 3){
        if (tempValue->type == 2){
          tempValue->type = 3;
          tempValue->floatValue = tempValue->intValue;
        }
        tempValue->floatValue = tempValue->floatValue + varVal->floatValue;
      }
      else {
        return newCreateNullVal();
      }
    }
    else {
      return newCreateNullVal();
    }
    if (expr->cdr->type != 8){
      return tempValue;
    }
    expr = expr->cdr->cons;
  }
  return tempValue;
}

// function that evaluates a / expression.
Value* evalDivision(ConsCell *expr, ConsCell *env){
  Value *numberOne;
  Value *numberTwo;
  Value *tempValue;
  expr = expr->cdr->cons;
  numberOne = eval(expr, env);
  numberTwo = eval(expr->cdr->cons, env);
  if ((numberOne->type == 3) && (numberTwo->type == 3)){
    tempValue = mallocValue();
    tempValue->type = 3;
    tempValue->floatValue = (numberOne->floatValue/ numberTwo->floatValue);
    return tempValue;
  }
  else if ((numberOne->type == 3) && (numberTwo->type == 2)){
    tempValue = mallocValue();
    tempValue->type = 3;
    tempValue->floatValue = (numberOne->floatValue/ numberTwo->intValue);
    return tempValue;
  }
  else if ((numberOne->type == 2) && (numberTwo->type == 3)){
    tempValue = mallocValue();
    tempValue->type = 3;
    tempValue->floatValue = (numberOne->intValue/ numberTwo->floatValue);
    return tempValue;
  }
  else if ((numberOne->type == 2) && (numberTwo->type == 2)){
    tempValue = mallocValue();
    if ((numberOne->intValue % numberTwo->intValue) == 0){
      tempValue->type = 2;
      tempValue->intValue = (numberOne->intValue / numberTwo->intValue);
      return tempValue;
    }
    else {
      tempValue->type = 3;
      tempValue->floatValue = ((numberOne->intValue + 0.0)/ numberTwo->intValue);
      return tempValue;
    }
    return tempValue;
  }
  else {
    return newCreateNullVal();
  }


}




// function that evaluates a * expression.
Value* evalMultiplication(ConsCell *expr, ConsCell* env){
  Value *tempValue;
  Value *varVal;
  
  if (expr->cdr->type != 8){
    tempValue = mallocValue();
    tempValue->type = 2;
    tempValue->intValue = 1;
    return tempValue;
  }
  expr= expr->cdr->cons;
  tempValue = mallocValue();
  tempValue->type = 0;
  tempValue->intValue = 0;

if (expr->car->type == 2){
    tempValue->type=2;
    tempValue->intValue=expr->car->intValue;
  }
else if (expr->car->type == 3){
    tempValue->type=3;
    tempValue->floatValue=expr->car->floatValue;
    }

  else if ((expr->car->type == 8) || (expr->car->type == 5) || (expr->car->type == 11)){
      varVal = eval(expr, env);
      if (varVal->type == 2){
          tempValue->type=2;
          tempValue->intValue=varVal->intValue;
      }
      else if (varVal->type == 3){
          tempValue->type = 3;
          tempValue->floatValue = varVal->floatValue;
        }
      else {
        return newCreateNullVal();
      }
    }
    else {
      return newCreateNullVal();
    }
    expr = expr->cdr->cons;

  while (expr!= NULL){
    if (expr->car->type == 2){
      if (tempValue->type == 2){
        tempValue->intValue = tempValue->intValue * expr->car->intValue;
      }
      else{
        tempValue->floatValue = tempValue->floatValue * expr->car->intValue;
      }
    }
    else if (expr->car->type == 3){
      if (tempValue->type == 2){
        tempValue->type = 3;
        tempValue->floatValue = tempValue->intValue;
      }
      tempValue->floatValue = tempValue->floatValue * expr->car->floatValue;
    }

    else if ((expr->car->type == 8) || (expr->car->type == 5) || (expr->car->type == 11)){
      varVal = eval(expr, env);
      if (varVal->type == 2){
        if (tempValue->type == 2){
          tempValue->intValue = tempValue->intValue * varVal->intValue;
        }
        else{
          tempValue->floatValue = tempValue->floatValue * varVal->intValue;
        }
      }
      else if (varVal->type == 3){
        if (tempValue->type == 2){
          tempValue->type = 3;
          tempValue->floatValue = tempValue->intValue;
        }
        tempValue->floatValue = tempValue->floatValue * varVal->floatValue;
      }
      else {
        return newCreateNullVal();
      }
    }
    else {
      return newCreateNullVal();
    }
    if (expr->cdr->type != 8){
      return tempValue;
    }
    expr = expr->cdr->cons;
  }
  return tempValue;
}


// function that evaluates a begin expression.
Value* evalBegin(ConsCell* expr, ConsCell* env){
  Value* tempValue;
  if (expr->cdr->type != 8){
    tempValue = mallocValue();
    tempValue->type = 10;
    tempValue->intValue = 0;
    return tempValue;
  }
  expr = expr->cdr->cons;
  while(expr != NULL){
    tempValue = eval(expr, env);
    if (expr->cdr->type != 8){
      return tempValue;
    }
    else {
      expr = expr->cdr->cons;
    }
  }
  return newCreateNullVal();
}


// function that evaluates a cond expression.
Value* evalCond(ConsCell* expr, ConsCell* env){
  Value* tempValue;
  Value* boolVal;
  if (expr->cdr->type != 8){
    tempValue = mallocValue();
    tempValue->type = 10;
    tempValue->intValue = 0;
    return tempValue;
  }
  expr = expr->cdr->cons;
  while(expr != NULL){
    if (expr->car->type != 8){
      return newCreateNullVal();
    }
    else{
      boolVal = eval(expr->car->cons, env);
      if (boolVal->type != 1) return newCreateNullVal();
      else {
        if (boolVal->boolValue == 1){
          if (expr->car->cons->cdr->type == 8){
            return eval(expr->car->cons->cdr->cons, env);
          }
        }
      }
    }
    if (expr->cdr->type != 8){
      tempValue = mallocValue();
      tempValue->type = 10;
      tempValue->intValue = 0;
      return tempValue;
    }
    else {
      expr = expr->cdr->cons;
    }
  }
  return newCreateNullVal();
}



// our main eval function.  Everything is evaluated starting here.  
Value* eval(ConsCell *expr, ConsCell *env) {
 Value* tempValue;
 switch (expr->car->type)  {
    case 0:
    return eval(expr->cdr->cons, env);
    break;
    case 1:
    tempValue = mallocValue();
    tempValue = copyValue(tempValue, expr->car);
    return tempValue;
    break;
    case 2:
    tempValue = mallocValue();
    tempValue = copyValue(tempValue, expr->car);
    return tempValue;
    break;
    case 3:
    tempValue = mallocValue();
    tempValue = copyValue(tempValue, expr->car);
    return tempValue;
    break;
    case 4:
    tempValue = mallocValue();
    tempValue = copyValue(tempValue, expr->car);
    return tempValue;
    break;
    case 5:
    
    tempValue = resolveVariable(expr->car, env);
    if (tempValue->type == 12){
      return applyFunc(tempValue, expr, env);
    }
    return tempValue;
    break;
    case 6:
    break;
    case 7:
    break;
    case 8:
    if (expr->car->cons->car->type == 0){
      return newCreateNullVal();
    }
    return eval(expr->car->cons, env);
    break;
    case idType:
      if (!strcmp(expr->car->idValue, "if")) {
        if (expr->cdr->type == 8){
          if (expr->cdr->cons->cdr->type == 8){
            if (expr->cdr->cons->cdr->cons->cdr->type == 8){
              if (expr->cdr->cons->cdr->cons->cdr->cons->cdr->type ==8){
                tempValue = mallocValue();
                tempValue->type = 0;
                tempValue->intValue = 0;
                return tempValue;
              }
              return evalIf(expr->cdr->cons,expr->cdr->cons->cdr->cons,expr->cdr->cons->cdr->cons->cdr->cons, env);
            }
            else {
              return newCreateNullVal();
            }
          }
          else{
            return newCreateNullVal();
          }
        }
        else{
          return newCreateNullVal();
        } 
      }
      else if (!strcmp(expr->car->idValue, "cond")){
        return evalCond(expr, env);
      }
      else if (!strcmp(expr->car->idValue, "begin")){
        return evalBegin(expr, env);
      }
      else if (!strcmp(expr->car->idValue, "or")) {
        return evalOr(expr, env);
      }
      else if (!strcmp(expr->car->idValue, "and")) {
        return evalAnd(expr, env);
      }
      else if (!strcmp(expr->car->idValue, "let")) {
        if ((expr->cdr->cons->car->type == 8) && (expr->cdr->cons->car->cons->car->type == 8)) {
         return evalLet(expr->cdr->cons->car->cons, expr->cdr->cons->cdr->cons, env);
        }
        else {
          return newCreateNullVal();
        }
      }
      else if (!strcmp(expr->car->idValue, "letrec")){
        if ((expr->cdr->cons->car->type == 8) && (expr->cdr->cons->car->cons->car->type == 8)) {
         return evalLetStar(expr->cdr->cons->car->cons, expr->cdr->cons->cdr->cons, env);
        }
        else {
          return newCreateNullVal();
        }
      }
      else if (!strcmp(expr->car->idValue, "let*")) {
        if ((expr->cdr->cons->car->type == 8) && (expr->cdr->cons->car->cons->car->type == 8)) {
         return evalLetStar(expr->cdr->cons->car->cons, expr->cdr->cons->cdr->cons, env);
        }
        else {
          return newCreateNullVal();
        }
      }
      else if (!strcmp(expr->car->idValue, "quote")) {
        return evalQuote(expr->cdr->cons);
      }
      else if (!strcmp(expr->car->idValue, "lambda")) {
        if (expr->cdr->type == 8){
          if (expr->cdr->cons->cdr->type == 8){
            if (expr->cdr->cons->cdr->cons->cdr->type == 8){
              return newCreateNullVal();
            }
            return evalLambda(expr->cdr->cons, expr->cdr->cons->cdr->cons, env);
          }
          else{
            return newCreateNullVal();
          }
        }
        else{
          return newCreateNullVal();
        }
      }
      else if (!strcmp(expr->car->idValue, "define")){

        if (expr->cdr->type == 8){
          if (expr->cdr->cons->cdr->type == 8){
            if (expr->cdr->cons->cdr->cons->cdr->type == 8){
              return newCreateNullVal();
            }
            
            return evalDefine(expr->cdr->cons, expr->cdr->cons->cdr->cons, env);
          }
          else{
            return newCreateNullVal();
          }
        }
        else{
          return newCreateNullVal();
        }
      }
      break;
      case 10:
      break;
      case 11:
      break;
      case 13:
      if (!strcmp(expr->car->primValue, "+")){
        return evalAddition(expr, env);
      }
      else if (!strcmp(expr->car->primValue, "-")){
        if (expr->cdr->type == 8){
          if (expr->cdr->cons->cdr->type == 8){
            if (expr->cdr->cons->cdr->cons->cdr->type == 8){
              return newCreateNullVal();
            }
            return evalSubtraction(expr, env);
            }
            else {
              return newCreateNullVal();
            }
          }
          else{
            return newCreateNullVal();
          }
        }
      
      else if (!strcmp(expr->car->primValue, "*")){
        return evalMultiplication(expr, env);
      }
      else if (!strcmp(expr->car->primValue, "/")){
        if (expr->cdr->type == 8){
          if (expr->cdr->cons->cdr->type == 8){
            if (expr->cdr->cons->cdr->cons->cdr->type == 8){
              return newCreateNullVal();
            }
            return evalDivision(expr, env);
            }
            else {
              return newCreateNullVal();
            }
          }
          else{
            return newCreateNullVal();
          }
        }
      else if (!strcmp(expr->car->primValue, "modulo")) {
        if (expr->cdr->type == 8){
          if (expr->cdr->cons->cdr->type == 8){
            if (expr->cdr->cons->cdr->cons->cdr->type == 8){
              return newCreateNullVal();
            }
            return evalModulo(expr, env);
            }
            else {
              return newCreateNullVal();
            }
          }
          else{
            return newCreateNullVal();
          }
        }
      else if (!strcmp(expr->car->primValue, "=")){
        if (expr->cdr->type == 8){
          if (expr->cdr->cons->cdr->type == 8){
            if (expr->cdr->cons->cdr->cons->cdr->type == 8){
              Value* tempValue = mallocValue();
              tempValue = setNullVal(tempValue);
              return tempValue;
            }
            return evalEquals(expr, env);
            }
            else {
              return newCreateNullVal();
            }
          }
          else{
            return newCreateNullVal();
          }
        }
      else if (!strcmp(expr->car->primValue, ">=")){
        if (expr->cdr->type == 8){
          if (expr->cdr->cons->cdr->type == 8){
            if (expr->cdr->cons->cdr->cons->cdr->type == 8){
              return newCreateNullVal();
            }
            return evalGreaterThanEqualTo(expr, env);
            }
            else {
              return newCreateNullVal();
            }
          }
          else{
            return newCreateNullVal();
          }
        }
      else if (!strcmp(expr->car->primValue, "<=")) {
        if (expr->cdr->type == 8){
          if (expr->cdr->cons->cdr->type == 8){
            if (expr->cdr->cons->cdr->cons->cdr->type == 8){
              return newCreateNullVal();
            }
            return evalLessThanEqualTo(expr, env);
            }
            else {
              return newCreateNullVal();
            }
          }
          else{
            return newCreateNullVal();
          }
        }
      else if (!strcmp(expr->car->primValue, ">")){
        if (expr->cdr->type == 8){
          if (expr->cdr->cons->cdr->type == 8){
            if (expr->cdr->cons->cdr->cons->cdr->type == 8){
              return newCreateNullVal();
            }
            return evalGreaterThan(expr, env);
            }
            else {
              return newCreateNullVal();
            }
          }
          else{
            return newCreateNullVal();
          }
        }
      else if (!strcmp(expr->car->primValue, "<")) {
        if (expr->cdr->type == 8){
          if (expr->cdr->cons->cdr->type == 8){
            if (expr->cdr->cons->cdr->cons->cdr->type == 8){
              return newCreateNullVal();
            }
            return evalLessThan(expr, env);
            }
            else {
              return newCreateNullVal();
            }
          }
          else{
            return newCreateNullVal();
          }
        }
      else if (!strcmp(expr->car->primValue, "null?")){
        if (expr->cdr->type == 8){
          if (expr->cdr->cons->cdr->type == 8){
            return newCreateNullVal();
          }
          expr = expr->cdr->cons;
          return evalNull(eval(expr, env), env);
        }
        else {
          return newCreateNullVal();
        }
      }
      else if (!strcmp(expr->car->primValue, "car")){

        if (expr->cdr->type == 8){
          if (expr->cdr->cons->cdr->type == 8){
            return newCreateNullVal();
          }
          expr = expr->cdr->cons;
          return evalCar(eval(expr, env), env);
        }
        else {
          return newCreateNullVal();
        }
      }
      else if (!strcmp(expr->car->primValue, "cdr")){
        
        if (expr->cdr->type == 8){
          if (expr->cdr->cons->cdr->type == 8){
            return newCreateNullVal();
          }
          expr = expr->cdr->cons;
          return evalCdr(eval(expr, env), env);
        }
        else {
          return newCreateNullVal();
        }
      }
      else if (!strcmp(expr->car->primValue, "cons")){
        if (expr->cdr->type == 8){
          if (expr->cdr->cons->cdr->type == 8){
            if (expr->cdr->cons->cdr->cons->cdr->type == 8){
              return newCreateNullVal();
            }
            Value *first = eval(expr->cdr->cons, env);
            Value *second = eval(expr->cdr->cons->cdr->cons, env);
            if ((first->type != 8) && (second->type != 8)){
              if ((first->type == 0) || (second->type == 0)) return newCreateNullVal();
              else return evalCons(first, second, env);
            }
            else {
              return newCreateNullVal();
            }
          }
          else{
            return newCreateNullVal();
          }
        }
        else{
          return newCreateNullVal();
        }
      }
      else {
        return newCreateNullVal();
      }
      break;
      default:
      break;
  }
  return newCreateNullVal();
}
     