#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define INPUT_MAX_SIZE 255
#define CMD_MAX_SIZE 100

void getCmd(char input[], char inputCmd[]);
bool isBuiltInCmd(char inputCmd[]);
bool isBinExist(const char *fileName);


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

const char *envPath = getenv("PATH");
#ifdef __unix__
	const char *envPathDelim = ":";
#elif _WIN32
	const char *envPathDelim = ";";
#endif

	while (!isExitShell) {
		char *envPathCpy = malloc(strlen(envPath));
		strcpy(envPathCpy, envPath);

		printf("$ ");

		fgets(userInput, sizeof(userInput), stdin);

		userInput[strcspn(userInput, "\n")] = '\0';
		getCmd(userInput, inputCmd);

		if (strncmp(inputCmd, "type", strlen("type")) == 0)
		{
			char *binaryName = userInput + strlen("type ");

			if (isBuiltInCmd(binaryName))
			{
				printf("%s is a shell builtin\n", binaryName);
			} else {
				bool isBinExistInPath = false;
				char *token = strtok(envPathCpy, envPathDelim);
				while (token != NULL && !isBinExistInPath)
				{
					char *binaryLoc = malloc(strlen(token) + strlen(binaryName) + 2);
					if (binaryLoc == NULL)
					{
						fprintf(stderr, "Error Allocating Memeory for binaryName");
						free(envPathCpy);
						return 1;
					}
					strcpy(binaryLoc, token);
					strcat(binaryLoc, "/");
					strcat(binaryLoc, (binaryName));
					strcat(binaryLoc, "\0");

					if (access(binaryLoc, F_OK) == 0)
					{
						isBinExistInPath = true;
						if (isBinExist(binaryLoc)) {
							printf("%s is %s\n", binaryName, binaryLoc);
						}
					}

					token = strtok(NULL, envPathDelim);
					free(binaryLoc);
				}
				if (!isBinExistInPath) 
				{
					fprintf(stderr, "%s: not found\n", binaryName);
				}
			}
		} else if (!strncmp(userInput, "exit", strlen("exit")))
		{
			isExitShell = true;
		} else if (!strncmp(userInput, "echo", strlen("echo"))) {
			printf("%s\n", userInput + strlen("echo "));	
		} else {
			fprintf(stderr, "%s: command not found\n", userInput);
		}
		free(envPathCpy);
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

bool isBinExist(const char *fileName)
{
	FILE *file = fopen(fileName, "rb");
	if (file == NULL)
	{
		return false;
	}
	fclose(file);
	return true;
}
