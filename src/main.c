#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  char userInput[100];

  printf("$ ");

  fgets(userInput, sizeof(userInput), stdin);

  userInput[strcspn(userInput, "\n")] = '\0';

  fprintf(stderr, "%s: command not found\n", userInput);

  return 0;
}
