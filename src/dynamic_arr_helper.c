#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "dynamic_arr_helper.h"

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
