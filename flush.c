#include <string.h>

/*
 * Program Name: flush.c
 * Author(s): Samantha Preisig
 * Purpose: clears data stored in the character array
 */

int flush ( char *input) {
  int inputLength;
  int i = 0;
  char character;

  inputLength = strlen (input);

  for (i = 0; i < inputLength; i++) {
    character = input[i];
    if (character != '\0') {
      input[i] = '\0';
    }
  }
  input[inputLength] = '\0';
  return(0);
}
