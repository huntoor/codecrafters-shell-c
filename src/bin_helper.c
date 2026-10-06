#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdarg.h>

#include "./bin_helper.h"

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
		if (isBinExist(binaryPath))
		{
			if (!isBinExec)
			{
				free(envPathCpy);
				return binaryPath;
			}

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
