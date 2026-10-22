#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <ctype.h>

#include "bin_helper.h"

#define INPUT_MAX_SIZE 255

void getCmd(char input[], char inputCmd[]);
bool isBuiltInCmd(char inputCmd[]);
char *getCWD();
char **userInputTokenizer(char *userInput);
char *getUserInput();

char **initializeDarrString(size_t *capacity);
int increaseSizeofDarrString(char ***arr, size_t arrLastIndex, size_t *capacity);
int addStringToDarr(char ***dstArr, char **srcArr, size_t *arrLastIndex, size_t srcLen);

char *initializeDarr(size_t *capacity);
int increaseSizeofDarr(char **arr, size_t *capacity);



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

	//char userInput[INPUT_MAX_SIZE];
	char *userInput;
	char *inputCmd;
	bool isExitShell = false;

	while (!isExitShell) {
		printf("$ ");

		//fgets(userInput, sizeof(userInput), stdin);
		userInput = getUserInput();

		//userInput[strcspn(userInput, "\n")] = '\0';
	
		char **inputArgs = userInputTokenizer(userInput);
		if (inputArgs[0] == NULL)
		{
			continue;
		}
		inputCmd = inputArgs[0];

		if (isBuiltInCmd(inputCmd)) // Look for built in CMDs
		{
			if (strncmp(inputCmd, "type", strlen("type")) == 0)
			{
				char *binaryName = inputArgs[1];

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
			} else if (!strncmp(inputCmd, "exit", strlen("exit")))
			{
				isExitShell = true;
			} else if (!strncmp(inputCmd, "echo", strlen("echo"))) {
				
				for (int i = 1; inputArgs[i]; i++)
				{
					printf("%s ", inputArgs[i]);
				}
				printf("\n");
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
				if (inputArgs[1] == NULL)
				{
					char *homePath = getenv("HOME");
					if (chdir(homePath) != 0)
					{
						fprintf(stderr, "cd: %s: No such file or directory\n", homePath);
					}
				} else {
					if (chdir(inputArgs[1]) != 0)
					{	
						fprintf(stderr, "cd: %s: No such file or directory\n", inputArgs[1]);
					}
				}
			}
		} else // look for the command in the PATH
		{
			char *binPathLoc = getBinPath(inputCmd, true);

			if (binPathLoc != NULL)
			{
				//executeBin(binPathLoc, userInput);
				executeBin2(binPathLoc, inputArgs);
			} else {
				fprintf(stderr, "%s: command not found\n", userInput);
			}
			free(binPathLoc);
		}

		free(inputArgs);
	}
	//free(userInput);

	return 0;
}

char **userInputTokenizer(char *userInput)
{
	enum parser_state {
		NORMAL,
		SINGLE_QUOTED, 
		DOUBLE_QUOTED, 
	} parser_state;

	enum parser_state parserState = NORMAL;
	
	size_t tmpCapacity = 255;
	size_t tmpIndex = 0;
	//char *tmpInput = malloc(tmpCapacity * sizeof(char));
	char *tmpInput = initializeDarr(&tmpCapacity);
	if (tmpInput == NULL)
	{
		perror("Error Allocating Memory for tokenizedInputArr");

		return NULL;
	}

	size_t capacity = 4;
	size_t index = 0;
	//char **tokenizedInputArr = malloc(capacity * sizeof(char *));
	char **tokenizedInputArr = initializeDarrString(&capacity);
	if (tokenizedInputArr == NULL)
	{
		perror("Error Allocating Memory for tokenizedInputArr");

		free(tmpInput);
		return NULL;
	}
	
	char *userInputCpy = malloc(strlen(userInput) + 1);
	if (userInputCpy == NULL)
	{
		perror("Error Allocating Memory for userInputCpy");

		free(tokenizedInputArr);
		free(tmpInput);
		return NULL;
	}
	strcpy(userInputCpy, userInput);

	for (char *c = userInputCpy; *c; c++)
	{
		char *prevChar = (c == userInputCpy) ? c : c - 1;
		char *nextChar = (*c == '\0') ? c : c + 1;
		if (index + 1 >= capacity)
		{
		//	capacity *= 2;
		//	 
		//	char **tmp = realloc(tokenizedInputArr, (capacity * sizeof(char *)));
		//	if (tmp == NULL)
		//	{
		//		perror("Error Allocating Memory in sanetizeUserInput\n");

		//		for (int i = 0; i < index; i++) {
		//			free(tokenizedInputArr[i]);
		//		}
		//		
		//		free(tmpInput);
		//		free(tokenizedInputArr);
		//		free(userInputCpy);
		//		return NULL;
		//	}
		//	tokenizedInputArr = tmp;
		//
			if (increaseSizeofDarrString(&tokenizedInputArr, index, &capacity) == -1)
			{
				free(tmpInput);
				free(tokenizedInputArr);
				free(userInputCpy);

				return NULL;
			}
		}


		if (tmpIndex + 1 >= tmpCapacity)
		{
			//tmpCapacity *= 2;
			// 
			//char *tmp = realloc(tmpInput, (tmpCapacity * sizeof(char)));
			//if (tmp == NULL)
			//{
			//	perror("Error Allocating Memory in tmpInput\n");
			//	
			//	free(tmpInput);
			//	free(tokenizedInputArr);
			//	free(userInputCpy);
			//	return NULL;
			//}
			//tmpInput = tmp;
			if (increaseSizeofDarr(&tmpInput, &tmpCapacity) == -1)
			{
				free(tokenizedInputArr);
				free(userInputCpy);

				return NULL;
			}
		}

		if (parserState == NORMAL)
		{
			if (*c == '\'')
			{
				parserState = SINGLE_QUOTED;

				continue;
			} else if (*c == '"')
			{
				parserState = DOUBLE_QUOTED;

				continue;
			}

			if (!isspace(*c))
			{
				if (*c == '~' && tmpIndex == 0)
				{
					char *homePath = getenv("HOME");

					if (homePath != NULL)
					{
						for (int i = 0; homePath[i]; i++)
						{
							if (tmpIndex + 1 >= tmpCapacity)
							{
							//	tmpCapacity *= 2;
							//	 
							//	char *tmp = realloc(tmpInput, (tmpCapacity * sizeof(char)));
							//	if (tmp == NULL)
							//	{
							//		perror("Error Allocating Memory in tmpInput\n");
							//		
							//		free(tmpInput);
							//		free(tokenizedInputArr);
							//		free(userInputCpy);
							//		return NULL;
							//	}
							//	tmpInput = tmp;
								
								if (increaseSizeofDarr(&tmpInput, &tmpCapacity) == -1)
								{
									free(tokenizedInputArr);
									free(userInputCpy);

									return NULL;
								}
							}

							tmpInput[tmpIndex] = homePath[i];
							tmpIndex++;
						}
					}

				} else {
					tmpInput[tmpIndex] = *c;
					tmpIndex++;	
				}
			}

			if (isspace(*c) && tmpIndex > 0)
			{
				tmpInput[tmpIndex] = '\0';
				tmpIndex = 0;

				if (addStringToDarr(&tokenizedInputArr, &tmpInput, &index, (strlen(tmpInput))) == -1)
				{
					perror("Error Allocating Memory tokenizedInputArr");	

					free(userInputCpy);
					free(tokenizedInputArr);
					return NULL;
				}

			//	tokenizedInputArr[index] = malloc(strlen(tmpInput) + 1);
			//	if (tokenizedInputArr[index] == NULL)
			//	{
			//		for (int i = 0; i < index; i++) {
			//			free(tokenizedInputArr[i]);
			//		}
			//		
			//		perror("Error Allocating Memory tokenizedInputArr");	

			//		free(tokenizedInputArr);
			//		free(userInputCpy);
			//		return NULL;

			//	}
			//	strcpy(tokenizedInputArr[index], tmpInput);
			//	index++;
			//	tmpIndex = 0;

				continue;
			}
		}

		
		if (parserState == SINGLE_QUOTED)
		{
			if (*c == '\'' && *nextChar == ' ')
			{
				if (tmpIndex > 0)
				{
					tmpInput[tmpIndex] = '\0';
					tmpIndex = 0;

					if (addStringToDarr(&tokenizedInputArr, &tmpInput, &index, (strlen(tmpInput))) == -1)
					{
						perror("Error Allocating Memory tokenizedInputArr");	

						free(userInputCpy);
						free(tokenizedInputArr);
						return NULL;
					}

				//	tokenizedInputArr[index] = malloc(strlen(tmpInput) + 1);
				//	if (tokenizedInputArr[index] == NULL)
				//	{
				//		for (int i = 0; i < index; i++) {
				//			free(tokenizedInputArr[index]);
				//		}
				//		
				//		perror("Error Allocating Memory tokenizedInputArr");	

				//		free(tokenizedInputArr);
				//		free(userInputCpy);
				//		return NULL;

				//	}
				//	strcpy(tokenizedInputArr[index], tmpInput);
				//	index++;
				//	tmpIndex = 0;
				}

				parserState = NORMAL;

				continue;
			}
			
			if (*c != '\'')
			{
				tmpInput[tmpIndex] = *c;
				tmpIndex++;
			}
		}

		if (parserState == DOUBLE_QUOTED)
		{
			if (*c == '"' && isspace(*nextChar))
			{
				if (tmpIndex > 0)
				{
					tmpInput[tmpIndex] = '\0';
					tmpIndex = 0;

					if (addStringToDarr(&tokenizedInputArr, &tmpInput, &index, (strlen(tmpInput))) == -1)
					{
						perror("Error Allocating Memory tokenizedInputArr");	

						free(userInputCpy);
						free(tokenizedInputArr);
						return NULL;
					}
				//	tokenizedInputArr[index] = malloc(strlen(tmpInput) + 1);
				//	if (tokenizedInputArr[index] == NULL)
				//	{
				//		for (int i = 0; i < index; i++) {
				//			free(tokenizedInputArr[index]);
				//		}
				//		
				//		perror("Error Allocating Memory tokenizedInputArr");	

				//		free(tokenizedInputArr);
				//		free(userInputCpy);
				//		return NULL;

				//	}
				//	strcpy(tokenizedInputArr[index], tmpInput);
				//	index++;
				//	tmpIndex = 0;
				}
				
				parserState = NORMAL;

				continue;
			}

			if (*c != '"')
			{
				tmpInput[tmpIndex] = *c;
				tmpIndex++;
			}

		}
	}
	
	if (tmpIndex != 0)
	{
		tmpInput[tmpIndex] = '\0';
		tmpIndex = 0;

		if (index + 1 > capacity)
		{
			if (increaseSizeofDarrString(&tokenizedInputArr, index, &capacity) == -1)
			{
				free(tmpInput);
				free(tokenizedInputArr);
				free(userInputCpy);

				return NULL;
			}
		}

		if (addStringToDarr(&tokenizedInputArr, &tmpInput, &index, (strlen(tmpInput))) == -1)
		{
			perror("Error Allocating Memory tokenizedInputArr");	

			free(userInputCpy);
			free(tokenizedInputArr);
			return NULL;
		}
	//	tokenizedInputArr[index] = malloc(strlen(tmpInput) + 1);
	//	if (tokenizedInputArr[index] == NULL)
	//	{
	//		for (int i = 0; i < index; i++) {
	//			free(tokenizedInputArr[index]);
	//		}
	//		
	//		perror("Error Allocating Memory tokenizedInputArr");	

	//		free(tokenizedInputArr);
	//		free(userInputCpy);
	//		return NULL;

	//	}
	//	strcpy(tokenizedInputArr[index], tmpInput);
	//	index++;
	}

	tokenizedInputArr[index] = NULL;

	free(userInputCpy);
	free(tmpInput);

//	for (int i = 0; i < index; i++)
//	{
//		printf("Token :%s\n", tokenizedInputArr[i]);
//	}

	return tokenizedInputArr;
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

char *getUserInput()
{
	int input;

	size_t capacity = 128;
	size_t index = 0;

	char *userInput = malloc(sizeof(char) * capacity);
	if (userInput == NULL) {
		perror("Memeory Allocating for userInput Failed");

		return NULL;
	}
	
	while (1) {
		input = getchar();
		if (index >= capacity) {
			capacity *= 2;

			char *tmp = realloc(userInput, (sizeof(char) * capacity));
			if (tmp == NULL) {
				perror("Memeory Allocating Failed");

				free(userInput);
				return NULL;
			}

			userInput = tmp;
		}

		if (input == '\n' || input == EOF)
		{
			userInput[index] = '\0';
			index++;

			break;
		}

		userInput[index] = input;
		index++;

	}

	return userInput;
}

char **initializeDarrString(size_t *capacity)
{
	char **dArr = malloc((*capacity) * sizeof(char *));
	if (dArr == NULL)
	{
		perror("Error Allocating Memory for tokenizedInputArr");

		return NULL;
	}

	return dArr;
}

int increaseSizeofDarrString(char ***arr, size_t arrLastIndex, size_t *capacity)
{
	*capacity *= 2;
	 
	char **tmp = realloc(*arr, ((*capacity) * sizeof(char *)));
	if (tmp == NULL)
	{
		perror("Error Allocating Memory in sanetizeUserInput\n");

		for (int i = 0; i < arrLastIndex; i++) {
			free((*arr)[i]);
		}
	
		return -1;
	}
	*arr = tmp;
	return 1;
}

char *initializeDarr(size_t *capacity)
{
	char *arr = malloc((*capacity) * sizeof(char));
	if (arr == NULL)
	{
		perror("Error Allocating Memeory");

		return NULL;
	}

	return arr;
}

int increaseSizeofDarr(char **arr, size_t *capacity)
{
	*capacity *= 2;

	char *tmp = realloc(*arr, ((*capacity) * sizeof(char)));
	if (tmp == NULL) {
		return  -1;
	}

	*arr = tmp;
	
	return 1;
}


int addStringToDarr(char ***dstArr, char **srcArr, size_t *arrLastIndex, size_t srcLen)
{
	(*dstArr)[*arrLastIndex] = malloc(srcLen + 1);
	if ((*dstArr)[*arrLastIndex] == NULL)
	{
		for (int i = 0; i < *arrLastIndex; i++) {
			free((*dstArr)[i]);
		}
		
		perror("Error Allocating Memory dstArr");	

		return -1;

	}
	strcpy((*dstArr)[*arrLastIndex], *srcArr);
	*arrLastIndex += 1;

	return 1;
}
