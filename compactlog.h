/* CompactLog Header */
#ifndef COMPACTLOG_H
#define COMPACTLOG_H

#include <stdbool.h>
#include <stdio.h>

#define COMMAND_NUM_ARGS 5
#define COMMAND_INPUT_FLAG "-i"
#define COMMAND_OUTPUT_FLAG "-o"
#define COMMAND_HELP_FLAG "-h"

#define MAX_LINE_LENGTH 250
#define DATE_SIZE 38

#define POSITION_OF_RES 98
#define POSITION_OF_RES_VAL 102
#define POSITION_OF_BATTERY_VAL 114

#define BATT_STR 10
#define RES_STR 20
#define RES_ARR_LEN 3

#define INT_MAX 2147483647
#define INT_MIN -2147483648

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
