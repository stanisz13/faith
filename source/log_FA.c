#include "log_FA.h"

#define getVarName(var) #var

void logS(const char* s)
{
    printf("%s\n", s);
}

void logError(const char* s)
{
    printf("[ERROR]: %s\n", s);
}

void logU(const unsigned s)
{
    printf("%u\n", s);
}

void logI(const int s)
{
    printf("%d\n", s);
}
void logF(const float s)
{
    printf("%f\n", s);
}

void newLine()
{
    printf("\n");
}

void logWarning(const char* s)
{
    printf("[WARNING]: %s\n", s);
}
