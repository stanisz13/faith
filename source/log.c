#include "log.h"

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
