// Copyright (C) 2025 Catherine Park - All Rights Reserved.
// You may use, distribute, and modify this code under the terms of the MIT license

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <stdbool.h>
#include <ctype.h>

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

bool compactLine(char*, char*);
char* isBatteryLine(char*, FILE*, FILE*);
void printHelp(int);
bool checkRes(char* res);
void storeRes(char*, char**);
void freeAll(char**, char*, char*, char*);
char* makeResString(char**);
int populateBatt(char*); 
char* makeBattString(int, int);
void printIntoFile(int, char*, FILE*);
void printBattIntoFile(char*, char**, int, int, FILE*);
void checkMalloc(void* ptr);

// Main function to handle command line arguments and initiate the compacting process
// It checks for the correct number of arguments and the correct flags
// If the command is incorrect, it prints the help message
// If the command is correct, it calls the compactLine function with the input and output file paths

int main(int argc, char *argv[]) {
    if (argc == 1 || (strcmp(argv[1], COMMAND_HELP_FLAG) == 0)) { // help syntax
        printHelp(0);
        return 0;
    }
    if (argc != COMMAND_NUM_ARGS ||
        (strcmp(argv[1], COMMAND_INPUT_FLAG) != 0) ||
        (strcmp(argv[3], COMMAND_OUTPUT_FLAG) != 0)) { // check if the command is correct
        printHelp(2);
        return 0;
    }
    bool res = compactLine(argv[2], argv[4]);
    if (res == false) {
        printf("Error processing files: %s, %s \n", argv[2], argv[4]);
        return 1; // return error code
    }
    printf("Task Complete! Come Again!\n");
    return 0;
}

// Function to compact log lines
// It reads the input file line by line, checks for duplicate lines, and writes the compacted output to the output file
// If a battery line is detected, it processes it using the isBatteryLine function
// Returns true if successful, false if there was an error opening files
// The inputF is the input file path and outputF is the output file path
bool compactLine(char* inputF, char* outputF) {
    FILE* inputFile = fopen(inputF,"r");
    FILE* outputFile = fopen(outputF, "w");
    if (NULL == inputFile || NULL == outputFile) {
        return false;
    }
    char* line = (char *) malloc(MAX_LINE_LENGTH);
    checkMalloc(line);
    char* prev = NULL;
    int count = 1;
    while (fgets(line, MAX_LINE_LENGTH, inputFile) && feof(inputFile) != EOF) {
        // if (strlen(line) < DATE_SIZE) {
        //     continue;
        // }
        line[strcspn(line, "\r\n")] = '\0';   // trim off \r and \n
        if (prev == NULL) {
            prev = strdup(line); 
        } else if (strcmp(line + DATE_SIZE, prev + DATE_SIZE) == 0) {
            count++;
        }  else {
            printIntoFile(count, prev, outputFile); // print the previous line and count
            if (checkRes(line)) { // check if the line is a battery line
                prev = strdup(isBatteryLine(line, outputFile, inputFile)); // process the battery line
            } else {
                prev = strdup(line);
            }
            count = 1;
        }
    }
    free(line);
    fclose(inputFile);
    fclose(outputFile);
    return true;
}
// Function to check if the line is a battery line and process it accordingly
// Returns the line if it is a battery line, otherwise returns NULL
// This function will also write the processed battery information to the output file
// The inputFile is used to read the next lines if needed
// The outputFile is used to write the processed battery information
char* isBatteryLine(char* line, FILE* outputFile, FILE* inputFile) {
    char** resArr = (char**) malloc(3 * sizeof(char*));
    checkMalloc(resArr);
    int battMin = INT_MAX; // initialize battMin to max int value
    int battMax = INT_MIN; // initialize battMax to min int value
    char* date = (char*) malloc(DATE_SIZE * sizeof(char));
    checkMalloc(date);
    strncpy(date, line, DATE_SIZE - 1);
    do {
        // if (strlen(line) < DATE_SIZE) {
        //     continue;
        // }
        line[strcspn(line, "\r\n")] = '\0'; // trim off \r and \n
        if (checkRes(line)) { // check if the line is a battery line
            storeRes(line, resArr);
            int thisBatt = populateBatt(line); // get the battery value
            if (thisBatt < battMin) {
                battMin = thisBatt; // update battMin
            } 
            if (thisBatt > battMax) {
                battMax = thisBatt; // update battMax
            }
        } else {
            printBattIntoFile(date, resArr, battMin, battMax, outputFile); // print the battery information to the output file
            return line; // return the line as is, no need to write to output file
        }
    } while (fgets(line, MAX_LINE_LENGTH, inputFile) && feof(inputFile) != EOF);
    return line;
}

void printHelp(int x) {
    if (x < 1) {
        printf("Arguments provided do not match given syntax.\n");
        printf("CompactLog is a command line tool that compacts multiple sequential same log lines into single line such as:\n\
            2024-11-20  8:22:29    18424:   32148 AppAssistManager::CheckAndHideWindow:exe=msedge.exe\n\
            2024-11-20  8:22:29    18424:   32148 AppAssistManager::CheckAndHideWindow:exe=msedge.exe\n\
            2024-11-20  8:22:29    18424:   32148 AppAssistManager::CheckAndHideWindow:exe=msedge.exe\n\
            To:\n\
            2024-11-20  8:22:29    18424:   32148 AppAssistManager::CheckAndHideWindow:exe=msedge.exe * {number of repeated}\n\n");
    }
    if (x <= 2) {
        printf("The command syntax is:\n\
            \nCompactLog -i  {input_logfile} -o {output_logfile} \n\
            CompactLog -h  // help syntax\
            \nCompactLog // help syntax\n");
    }
}

bool checkRes(char* line) {
    if (strlen(line) > POSITION_OF_RES + 4) {
        if (line[POSITION_OF_RES] == 'r' &&
            line[POSITION_OF_RES + 1] == 'e' &&
            line[POSITION_OF_RES + 2] == 's' && 
            line[POSITION_OF_RES + 3] == '=') {
            return true;
        }
    }
    return false;
}
void storeRes(char* line, char** resArr) {
    if (line[POSITION_OF_RES_VAL + 2] == '7') {
        resArr[0] = malloc(5 * sizeof(char));
        checkMalloc(resArr[0]);
        strcpy(resArr[0], "0x7");
    } else {
        resArr[0] = NULL; // no 0x7
    }
    if (line[POSITION_OF_RES_VAL + 2] == '1' && line[POSITION_OF_RES_VAL + 3] == 'c') {
        resArr[1] = malloc(5 * sizeof(char));
        checkMalloc(resArr[1]);
        strcpy(resArr[1], "0x1c");
    } else {
        resArr[1] = NULL; // no 0x1c
    }
    if (line[POSITION_OF_RES_VAL + 2] == '6') {
        resArr[2] = malloc(5 * sizeof(char));
        checkMalloc(resArr[2]);
        strcpy(resArr[2], "0x6");
    } else {
        resArr[2] = NULL; // no 0x6
    }
}

void freeAll(char** resArr, char* resString, char* battString, char* date) {
    for (int i = 0; i < RES_ARR_LEN; i++) {
        if (resArr[i] != NULL) {
            free(resArr[i]);
        }
    }
    free(resArr);
    free(resString);
    free(battString);
    free(date);
}

char* makeResString(char** resArr) {
    char* resString = (char*) malloc(RES_STR * sizeof(char));
    checkMalloc(resString);
    for (int i = 0; i < RES_ARR_LEN; i++) {
        if (resArr[i] != NULL) {
            strcpy(resString, resArr[i]); // free the memory allocated for resArr
        }
    }
    return resString;
}

int populateBatt(char* line) {
    char battString[BATT_STR]; // free later
    int battInd = 0;
    for (int i = POSITION_OF_BATTERY_VAL; i <= strlen(line); i++) { // populate battString with the value of battery
        if (i == strlen(line) || line[i] == '\n' || line[i] == ' ') {
            battString[battInd] = '\0'; // null terminate battString
            int battVal = atoi(battString);
            return battVal; // return the battery value
        } else if ('=' != line[i]) { // 
            battString[battInd] = line[i]; // append to battString
            battInd++;
        }
    }
    return 0;
}

char* makeBattString(int battMin, int battMax) {
    char* battString = (char*) malloc(BATT_STR * sizeof(char)); // allocate memory for battString
    checkMalloc(battString);
    if (battMax == battMin || battMin == INT_MAX) {
        sprintf(battString, "%d", battMax);
    } else if (battMax == INT_MIN) {
        sprintf(battString, "%d", battMin);
    } else {
        sprintf(battString, "%d-%d", battMin, battMax);
    }
    return battString;
}

void printIntoFile(int count, char* line, FILE* outputFile) {
    // printf("Printing into file\n"); // debug print
    if (strlen(line) <= DATE_SIZE) {
        return;
    }
    if (count > 1) {
        fprintf(outputFile,"%s * %d\n", line, count);
    } else {
        fprintf(outputFile, "%s\n", line);
    }
    fflush(outputFile);
}

void printBattIntoFile(char* date, char** resArr, int battMin, int battMax, FILE* outputFile) {
    char* resString = makeResString(resArr);
    char* battString = makeBattString(battMin, battMax);
    fprintf(outputFile, 
        "%s NotifyManager::ReportStatusAndWait:curl_easy_perform failed res=%s battery=%s\n", date, resString, battString);
    fflush(outputFile);
    freeAll(resArr, resString, battString, date); // free the memory allocated for resArr, resString, battString, and date
}

void checkMalloc(void* ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
}