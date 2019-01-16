#ifndef LOG_FA_H
#define LOG_FA_H

#include <stdio.h>

void logS(const char* s);

void logError(const char* s);

void logWarning(const char* s);

void logU(const unsigned s);

void logI(const int s);

void logF(const float f);

void newLine();

#endif
