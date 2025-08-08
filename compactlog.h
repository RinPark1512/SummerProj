/* CompactLog Header */
#ifndef COMPACTLOG_H
#define COMPACTLOG_H

#include <stdbool.h>
#include <stdio.h>

bool checkRes(char* line);
void storeRes(char* line, char** resArr);
char* makeResString(char** resArr);
char* makeBattString(int battMin, int battMax);
int populateBatt(char* line);
void checkMalloc(void* ptr); // Optional to test

bool compactLine(char*, char*);
char* isBatteryLine(char*, FILE*, FILE*);
void printHelp(int);
void freeAll(char**, char*, char*, char*);
void printIntoFile(int, char*, FILE*);
void printBattIntoFile(char*, char**, int, int, FILE*);

#endif
