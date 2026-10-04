#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdarg.h>

#define INPUT_MAX_SIZE 255
#define CMD_MAX_SIZE 100

void getCmd(char input[], char inputCmd[]);
bool isBuiltInCmd(char inputCmd[]);
bool isBinReadable(const char *fileName);
bool isBinExecutable(const char* fileName);
bool isBinWritable(const char* fileName);
bool isBinExist(const char *fileName);
char *getBinPath(const char *binName, bool isBinExec);
void executeBin(const char* inputCmdPath, const char *userInput);

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

bool isBinReadable(const char *fileName)
{
	FILE *file = fopen(fileName, "rb");
	if (file == NULL)
	{
		return false;
	}
	fclose(file);
	return true;
}

bool isBinExecutable(const char *fileName)
{
	//printf("FILENAME: %s\n", fileName);
	if (access(fileName, X_OK) == 0)
	{
		return true;
	}
	return false;
}

bool isBinWritable(const char *fileName)
{
	if (access(fileName, W_OK) == 0)
	{
		return true;
	}
	return false;
}

bool isBinExist(const char *fileName)
{
	if (access(fileName, F_OK) == 0)
	{
		return true;
	}
	return false;
}

char *getBinPath(const char *binName, bool isBinExec)
{
	const char *envPath = getenv("PATH");
#ifdef __unix__
	const char *envPathDelim = ":";
#elif _WIN32
	const char *envPathDelim = ";";
#endif

	char *envPathCpy = malloc(strlen(envPath));
	strcpy(envPathCpy, envPath);

	bool foundBin = false;

	char *token = strtok(envPathCpy, envPathDelim);
	while (token != NULL)
	{
		char *binaryPath = malloc(strlen(token) + strlen(binName) + 2);
		if (binaryPath == NULL)
		{
			fprintf(stderr, "Error Allocating Memeory for binaryName");
			free(envPathCpy);
			return NULL;
		}
		strcpy(binaryPath, token);
		strcat(binaryPath, "/");
		strcat(binaryPath, binName);
		strcat(binaryPath, "\0");
// if isBinExec is set the we need to return the bin that is executable or return empty string
// if isBinExec is not set then just return bin the exists
		if (!isBinExec && isBinExist(binaryPath))
		{	
			free(envPathCpy);
			return binaryPath;
		}	

		if (isBinExist(binaryPath))
		{
			foundBin = true;
			if (isBinExecutable(binaryPath))
			{
				free(envPathCpy);
				return binaryPath;
			}
		}

		token = strtok(NULL, envPathDelim);
		free(binaryPath);
	}

	if (foundBin)
	{
		free(envPathCpy);
		return "";
	}

	free(envPathCpy);
	return NULL;
}

void executeBin(const char* inputCmdPath, const char *userInput)
{
	char *userInputCpy = malloc(strlen(userInput));
	strcpy(userInputCpy, userInput);

	char **args = NULL;
	int index = 0;

	char *token = strtok(userInputCpy, " ");
	while (token != NULL)
	{
		char **tmp = realloc(args, ((index + 2) * sizeof(char*)));
		if (tmp == NULL)
		{
			free(args);
			free(userInputCpy);

			perror("Error Allocating Memeory for args");
			return;
		}
	
		args = tmp;
		args[index] = token;
		index++;
		args[index] = NULL;

		token = strtok(NULL, " ");
	}

	pid_t pid = fork();

	if (pid == 0)
	{
		int exevRes = execv(inputCmdPath, args);
	} else {
		waitpid(pid, NULL, 0);
	}
	
	free(args);
	free(userInputCpy);
}
