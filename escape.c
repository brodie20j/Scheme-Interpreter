//William Schifeling
/* Implementation of the escape and unescape functions */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "escape.h"
#include "interpret.h"




/* This method replaces special characters in the string with their
   escaped two-character versions. */
char *escape(char *unescaped) {
  int i;
  int shift = 0;
  int j = 1;

  //iterates counting to count how much I should allocate using malloc.
  for (i = 0; unescaped[i]; i++){
  	if (unescaped[i] == *"\n"|| unescaped[i] == *"\t" || unescaped[i] == *"\'" || unescaped[i] == *"\"" || unescaped[i] == *"\\"){
  		j = j + 1;
  	}
  }
  j = j + strlen(unescaped);
  char *newString = malloc(j*sizeof(char));
  void* pointer=newString;
  addToFreeList(pointer);

  //goes through unescaped checking for each escape charater to double it.
  for (i = 0; unescaped[i]; i++){
    if (unescaped[i] == *"\n"){
      newString[i + shift] = *"\\";
      shift = shift + 1;
      newString[i + shift] = *"n"; 
    }
    else if (unescaped[i] == *"\t"){
      newString[i + shift] = *"\\";
      shift = shift + 1;
      newString[i + shift] = *"t";
    }
    else if (unescaped[i] == *"\\"){
      newString[i + shift] = *"\\";
      shift = shift + 1;
      newString[i + shift] = *"\\";
    }
    else if (unescaped[i] == *"\'"){
      newString[i + shift] = *"\\";
      shift = shift + 1;
      newString[i + shift] = *"'";
    }
    else if (unescaped[i] == *"\""){
      newString[i + shift] = *"\\";
      shift = shift + 1;
      newString[i + shift] = *"\"";
    }
    else{
      newString[i + shift] = unescaped[i];
    }
  }
  newString[i + shift] = *"\0";
  return newString;

}



/* This method replaces escaped characters with their one-character
   equivalents. */
char *unescape(char *escaped) {
  int i;
  int shift = 0;
  int j = strlen(escaped);

  // This iterates through escape to see how much memory unescaped will need.
  for (i = 0; escaped[i]; i++){
    if (escaped[i] == *"\n"|| escaped[i] == *"\t" || escaped[i] == *"\'" || escaped[i] == *"\"" || escaped[i] == *"\\"){
      j = j -1;
    }
  }
  j = j + 2;
  char *unescaped = malloc(j*sizeof(char));
  void* pointer=unescaped;
  addToFreeList(pointer);
  int k = strlen(escaped) + 1;
  // goes through switching out the double escapes for one.
  for (i = 0; escaped[i]; i++){
    if ((i + shift) == k){
      break;
    }
    if (escaped[i + shift] == *"\\"){
      if (escaped[i +shift +1] == *"n"){
        unescaped[i] = *"\n";
        shift = shift + 1;
      }
      else if (escaped[i +shift +1] == *"t"){
        unescaped[i] = *"\t";
        shift = shift + 1;
      }
      else if (escaped[i +shift +1] == *"'"){
        unescaped[i] = *"\'";
        shift = shift + 1;
      }
      else if (escaped[i +shift +1] == *"\\"){
        unescaped[i] = *"\\";
        shift = shift + 1;
      }
      else if (escaped[i +shift +1] == *"\""){
        unescaped[i] = *"\"";
        shift = shift + 1;
      }
    }
    else if (i<j){
      unescaped[i] = escaped[i + shift];
    }
    else{
      break;
    }
  }
  unescaped[j-1] = *"\0";
  
  return unescaped;
  }
  



