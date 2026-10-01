#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define INPUT_MAX_SIZE 255


int main(int argc, char *argv[]) {
	// Flush after every printf
	setbuf(stdout, NULL);

	char userInput[INPUT_MAX_SIZE];
	bool isExitShell = false;

	while (!isExitShell) {
		printf("$ ");

		fgets(userInput, sizeof(userInput), stdin);

		userInput[strcspn(userInput, "\n")] = '\0';

		int i = 0;


		if (!strncmp(userInput, "exit", strlen("exit")))
		{
			isExitShell = true;
		} else if (!strncmp(userInput, "echo", strlen("echo"))) {
			printf("%s\n", userInput + strlen("echo "));	
		} else {
			fprintf(stderr, "%s: command not found\n", userInput);
		}
	}

	return 0;
}

