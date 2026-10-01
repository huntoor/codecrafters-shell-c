#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define INPUT_MAX_SIZE 255
#define CMD_MAX_SIZE 100

void getCmd(char input[], char inputCmd[]);
bool isBuiltInCmd(char inputCmd[]);


char *builtInCmds[] = 
{
	"echo",
	"exit",
	"type"
};
int builtInCmdLen = (sizeof(builtInCmds) / sizeof(builtInCmds[0])) - 1;

int main(int argc, char *argv[]) {
	// Flush after every printf
	setbuf(stdout, NULL);

	char userInput[INPUT_MAX_SIZE];
	char inputCmd[CMD_MAX_SIZE];
	bool isExitShell = false;
	

	while (!isExitShell) {
		printf("$ ");

		fgets(userInput, sizeof(userInput), stdin);

		userInput[strcspn(userInput, "\n")] = '\0';
		getCmd(userInput, inputCmd);

		if (strncmp(inputCmd, "type", strlen("type")) == 0)
		{
			if (isBuiltInCmd(userInput + strlen("type ")))
			{
				printf("%s is a shell builtin\n", userInput + strlen("type "));
			} else {
				fprintf(stderr, "%s: not found\n", userInput + strlen("type "));
			}
		} else if (!strncmp(userInput, "exit", strlen("exit")))
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

void getCmd(char input[], char inputCmd[])
{
	int endOfCmd = strcspn(input, " ");
	int i = 0;

	while (i < endOfCmd) {
		inputCmd[i] = input[i];

		i++;
	}
	inputCmd[i] = '\0';
}

bool isBuiltInCmd(char inputCmd[])
{
	for (int i = 0; i <= builtInCmdLen; i++)
	{
		if (!strcmp(inputCmd, builtInCmds[i]))
		{
			return true;
		}
	}	
	return false;	
}
