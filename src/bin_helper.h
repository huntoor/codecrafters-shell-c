


bool isBinReadable(const char *fileName);
bool isBinExecutable(const char* fileName);
bool isBinWritable(const char* fileName);
bool isBinExist(const char *fileName);
char *getBinPath(const char *binName, bool isBinExec);
void executeBin(const char* inputCmdPath, const char *userInput);
