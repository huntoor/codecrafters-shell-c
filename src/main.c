#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdarg.h>

#include "bin_helper.h"

#define INPUT_MAX_SIZE 255
//#define CMD_MAX_SIZE 100

void getCmd(char input[], char inputCmd[]);
bool isBuiltInCmd(char inputCmd[]);
char *getCWD();
char **sanetizeUserInput(char userInput[]);
char **userInputTokenizer(char userInput[]);

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
	char *inputCmd;
	bool isExitShell = false;

	while (!isExitShell) {
		printf("$ ");

		fgets(userInput, sizeof(userInput), stdin);

		userInput[strcspn(userInput, "\n")] = '\0';
		//getCmd(userInput, inputCmd);
	
		char **inputArgs = userInputTokenizer(userInput);
		inputCmd = inputArgs[0];

		if (isBuiltInCmd(inputCmd)) // Look for built in CMDs
		{
			if (strncmp(inputCmd, "type", strlen("type")) == 0)
			{
				//char *binaryName = userInput + strlen("type ");
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
				//printf("%s\n", userInput + strlen("echo "));	
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
				//char *inputPath = malloc(strlen(userInput + strlen("cd ")));
				
				//strcpy(inputPath, (userInput + strlen("cd ")));

			//	if (inputPath[0] == '~')
			//	{
			//		char *homePath = getenv("HOME");
			//		char *tmpPath = malloc((strlen(inputPath) + strlen(homePath) + 2));
			//		
			//		strcpy(tmpPath, homePath);
			//		strcat(tmpPath, (inputPath + strlen("~")));
			//		strcat(tmpPath, "\0");
			//		
			//		if (chdir(tmpPath) != 0)
			//		{
			//			fprintf(stderr, "cd: %s: No such file or directory\n", tmpPath);
			//		}

			//		free(tmpPath);
			//	} else if (chdir(inputPath) != 0)
			//	{
			//		fprintf(stderr, "cd: %s: No such file or directory\n", inputPath);
			//	}

			//	free(inputPath);

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
	}

	return 0;
}

char **userInputTokenizer(char userInput[])
{
	enum parser_state {
		NORMAL,
		SINGLE_QUOTED, 
	} parser_state;

	enum parser_state parserState = NORMAL;

	size_t capacity = 4;
	size_t index = 0;
	
	char tmpInput[INPUT_MAX_SIZE];
	size_t tmpIndex = 0;

	char **tokenizedInputArr = malloc(capacity * sizeof(char *));
	if (tokenizedInputArr == NULL)
	{
		perror("Error Allocating Memory for tokenizedInputArr");

		return NULL;
	}
	
	char *userInputCpy = malloc(strlen(userInput) + 1);
	if (userInputCpy == NULL)
	{
		perror("Error Allocating Memory for userInputCpy");

		free(tokenizedInputArr);
		return NULL;
	}
	strcpy(userInputCpy, userInput);

	for (char *c = userInputCpy; *c; c++)
	{
		char *prevChar = (c == userInputCpy) ? c : c - 1;
		char *nextChar = (*c == '\0') ? c : c + 1;
		if (capacity <= index)
		{
			capacity *= 2;
			 
			char **tmp = realloc(tokenizedInputArr, (capacity * sizeof(char *)));
			if (tmp == NULL)
			{
				perror("Error Allocating Memory in sanetizeUserInput\n");

				for (int i = 0; i < index; i++) {
					free(tokenizedInputArr[i]);
				}
				
				free(tokenizedInputArr);
				free(userInputCpy);
				return NULL;
			}
			tokenizedInputArr = tmp;
		}

		if (parserState == NORMAL)
		{
			//printf("Normal Statema\n");
			if (*c == '\'')
			{
				parserState = SINGLE_QUOTED;

				continue;
			}

			if (*c != ' ')
			{
				if (*c == '~' && tmpIndex == 0)
				{
					char *homePath = getenv("HOME");

					for (int i = 0; homePath[i]; i++)
					{
						tmpInput[tmpIndex] = homePath[i];
						tmpIndex++;
					}

				} else {
					tmpInput[tmpIndex] = *c;
					tmpIndex++;	
				}
			}

			if (*c == ' ' && tmpIndex > 0)
			{
				tmpInput[tmpIndex] = '\0';

				tokenizedInputArr[index] = malloc(strlen(tmpInput) + 1);
				if (tokenizedInputArr[index] == NULL)
				{
					for (int i = 0; i < index; i++) {
						free(tokenizedInputArr[index]);
					}
					
					perror("Error Allocating Memory tokenizedInputArr");	

					free(tokenizedInputArr);
					free(userInputCpy);
					return NULL;

				}
				strcpy(tokenizedInputArr[index], tmpInput);
				index++;
				tmpIndex = 0;

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

					tokenizedInputArr[index] = malloc(strlen(tmpInput) + 1);
					if (tokenizedInputArr[index] == NULL)
					{
						for (int i = 0; i < index; i++) {
							free(tokenizedInputArr[index]);
						}
						
						perror("Error Allocating Memory tokenizedInputArr");	

						free(tokenizedInputArr);
						free(userInputCpy);
						return NULL;

					}
					strcpy(tokenizedInputArr[index], tmpInput);
					index++;
					tmpIndex = 0;
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
	}
	
	if (tmpIndex != 0)
	{
		tmpInput[tmpIndex] = '\0';

		tokenizedInputArr[index] = malloc(strlen(tmpInput) + 1);
		if (tokenizedInputArr[index] == NULL)
		{
			for (int i = 0; i < index; i++) {
				free(tokenizedInputArr[index]);
			}
			
			perror("Error Allocating Memory tokenizedInputArr");	

			free(tokenizedInputArr);
			free(userInputCpy);
			return NULL;

		}
		strcpy(tokenizedInputArr[index], tmpInput);
		index++;
		tmpIndex = 0;
	}

	tokenizedInputArr[index] = NULL;

	//free(tokenizedInputArr);
	free(userInputCpy);

	//for (int i = 0; i < index; i++)
	//{
		//printf("Token :%s\n", tokenizedInputArr[i]);
	//}

	return tokenizedInputArr;
}

char **sanetizeUserInput(char userInput[])
{
	size_t capacity = 4;
	size_t index = 0;

	char **sanetizedArr = malloc(capacity * sizeof(char *));
	if (sanetizedArr == NULL)
	{
		perror("Error Allocating Memory for sanetizedArr\n");

		return NULL;
	}

	char *userInputCpy = malloc(strlen(userInput) + 1);
	if (userInputCpy == NULL)
	{
		perror("Error Allocating Memory for userInputCpy\n");
		
		free(sanetizedArr);
		return NULL;
	}
	strcpy(userInputCpy, userInput);

	char *token = strtok(userInputCpy, " \t\n");

	while (token != NULL) {
		
		if (capacity <= index)
		{
			capacity *= 2;
			 
			char **tmp = realloc(sanetizedArr, (capacity * sizeof(char *)));
			if (tmp == NULL)
			{
				perror("Error Allocating Memory in sanetizeUserInput\n");

				for (int i = 0; i < index; i++) {
					free(sanetizedArr[i]);
				}
				
				free(sanetizedArr);
				free(userInputCpy);
				return NULL;
			}

			sanetizedArr = tmp;
		}

		if (token[0] == '\'')
		{
			token = strtok(NULL, "\'");
			continue;
		}

		sanetizedArr[index] = malloc(strlen(token) + 1);
		if (sanetizedArr[index] == NULL)
		{
			perror("Error Allocating Memory for sanetizedArr[]\n");

			for (int i = 0; i < index; i++) {
				free(sanetizedArr[i]);
			}

			free(sanetizedArr);
			free(userInputCpy);
			return NULL;
		}
		strcpy(sanetizedArr[index], token);
		index++;
		
		token = strtok(NULL, " \t\n");
	}
	
	sanetizedArr[index] = NULL;

	index = 0;

	free(userInputCpy);

	return sanetizedArr;
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

//. Maybe you could use this someday
// if (*c == '\'' && *prevChar == ' ' && tmpIndex == 0)
// 		{
// 			c++;
// 			nextChar++;

// 			while ((*c != '\'' || *nextChar != ' ') && *c != '\0') {
// 				if (*c == '\'')
// 				{
// 					c++;
// 					nextChar++;

// 					continue;
// 				}

// 				tmpInput[tmpIndex] = *c;
// 				tmpIndex++;

// 				c++;
// 				nextChar++;
// 			}

// 			if (tmpIndex > 0)
// 			{
// 				tmpInput[tmpIndex] = '\0';

// 				tokenizedInputArr[index] = malloc(strlen(tmpInput) + 1);
// 				if (tokenizedInputArr[index] == NULL)
// 				{
// 					for (int i = 0; i < index; i++) {
// 						free(tokenizedInputArr[index]);
// 					}
					
// 					perror("Error Allocating Memory tokenizedInputArr");	

// 					free(tokenizedInputArr);
// 					free(userInputCpy);
// 					return NULL;

// 				}
// 				strcpy(tokenizedInputArr[index], tmpInput);
// 				index++;
// 				tmpIndex = 0;
// 			}
// 			continue;
// 			printf("Last C is %c\n", *c);
// 		}

// 		if (*c != ' ')
// 		{
// 			// Add char to tmpInput
// 			if (*c != '\'')
// 			{
// 				tmpInput[tmpIndex] = *c;
// 				tmpIndex++;
// 			}

// 		} else {
// 			// cpy tmpInput to tokinzedInputArr if tmpIndex > 0 else skip the space character
// 			if (tmpIndex > 0)
// 			{
// 				tmpInput[tmpIndex] = '\0';

// 				tokenizedInputArr[index] = malloc(strlen(tmpInput) + 1);
// 				if (tokenizedInputArr[index] == NULL)
// 				{
// 					for (int i = 0; i < index; i++) {
// 						free(tokenizedInputArr[index]);
// 					}
					
// 					perror("Error Allocating Memory tokenizedInputArr");	

// 					free(tokenizedInputArr);
// 					free(userInputCpy);
// 					return NULL;

// 				}
// 				strcpy(tokenizedInputArr[index], tmpInput);
// 				index++;
// 				tmpIndex = 0;
// 			}
// 		}
