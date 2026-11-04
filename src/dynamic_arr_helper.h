#include <stdlib.h>

char **initializeDarrString(size_t *capacity);
int increaseSizeofDarrString(char ***arr, size_t arrLastIndex, size_t *capacity);
int addStringToDarr(char ***dstArr, char **srcArr, size_t *arrLastIndex, size_t srcLen);

char *initializeDarr(size_t *capacity);
int increaseSizeofDarr(char **arr, size_t *capacity);
