//interpret.c
#include <stdbool.h>
#include <string.h>
#include "tokens.h"
#include "parse.h"
#include <stdio.h>
#include "interpret.h"


int main(){
	ConsCell* number;
	number = malloc(sizeof(ConsCell));
	void* pointer = number;
	void* freeArray = malloc(sizeof(int)*10000);
	free(pointer);
	free(freeArray);
	return 0;
}
