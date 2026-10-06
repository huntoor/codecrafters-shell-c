#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdarg.h>

#include "bin_helper.h"

#define INPUT_MAX_SIZE 255
#define CMD_MAX_SIZE 100

void getCmd(char input[], char inputCmd[]);
bool isBuiltInCmd(char inputCmd[]);
char *getCWD();

char *builtInCmds[] = 
{
	"echo",
	"exit",
	"type",
	"pwd",
	"cd",
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

		if (isBuiltInCmd(inputCmd)) // Look for the CMD in here
		{
			if (strncmp(inputCmd, "type", strlen("type")) == 0)
			{
				char *binaryName = userInput + strlen("type ");

				char *binaryLoc = getBinPath(binaryName, true);

				if (isBuiltInCmd(binaryName))
				{
					printf("%s is a shell builtin\n", binaryName);
				} else {	
					if (binaryLoc == NULL)
					{
						fprintf(stderr, "%s: not found\n", binaryName);
					} else if (strcmp(binaryLoc, "") != 0)
					{
						printf("%s is %s\n", binaryName, binaryLoc);
					}
				}
			//	free(binaryLoc);
			} else if (!strncmp(userInput, "exit", strlen("exit")))
			{
				isExitShell = true;
			} else if (!strncmp(userInput, "echo", strlen("echo"))) {
				printf("%s\n", userInput + strlen("echo "));	
			} else if (strncmp(inputCmd, "pwd", strlen("pwd")) == 0)
			{
				char *cwd = getcwd(NULL, 0);

				if (cwd != NULL)
				{
					printf("%s\n", cwd);
					free(cwd);
				} else {
					perror("Error getting current working Dir");
				}

			} else if (strncmp(inputCmd, "cd", strlen("cd")) == 0)
			{
				char *inputPath = malloc(strlen(userInput + strlen("cd ")));
				
				strcpy(inputPath, (userInput + strlen("cd ")));

				if (inputPath[0] == '~')
				{
					char *homePath = getenv("HOME");
					char *tmpPath = malloc((strlen(inputPath) + strlen(homePath) + 2));
					
					strcpy(tmpPath, homePath);
					strcat(tmpPath, (inputPath + strlen("~")));
					strcat(tmpPath, "\0");
					
					if (chdir(tmpPath) != 0)
					{
						fprintf(stderr, "cd: %s: No such file or directory\n", tmpPath);
					}

					free(tmpPath);
				} else if (chdir(inputPath) != 0)
				{
					fprintf(stderr, "cd: %s: No such file or directory\n", inputPath);
				}

				free(inputPath);
			}
		} else // look for the command in the PATH
		{
			char *binPathLoc = getBinPath(inputCmd, true);

			if (binPathLoc != NULL)
			{
				executeBin(binPathLoc, userInput);
			} else {
				fprintf(stderr, "%s: command not found\n", userInput);
			}
			free(binPathLoc);
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
 
