#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  char userInput[100];
  bool isExitShell = false;

	
  while (!isExitShell) {
  	printf("$ ");

	fgets(userInput, sizeof(userInput), stdin);

	userInput[strcspn(userInput, "\n")] = '\0';

	if (!strcmp(userInput, "exit"))
	{
		isExitShell = true;
	} else {
		fprintf(stderr, "%s: command not found\n", userInput);
	}
  }


  return 0;
}
