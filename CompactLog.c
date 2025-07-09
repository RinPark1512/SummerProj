// Copyright (C) 2025 Catherine Park - All Rights Reserved.
// You may use, distribute, and modify this code under the terms of the MIT license

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <stdbool.h>

#define COMMAND_NUM_ARGS 5
#define COMMAND_INPUT_FLAG "-i"
#define COMMAND_OUTPUT_FLAG "-o"
#define COMMAND_HELP_FLAG "-h"

#define MAX_LINE_LENGTH 200
#define DATE_SIZE 38

#define POSITION_OF_RES 60
#define POSITION_OF_RES_VAL 64
#define POSITION_OF_BATTERY_VAL 76

#define DW_CONTROL_POSITION 53
#define DW_EVENT_AGENT_LEN 11



bool compactLine(char*, char*);
char* isBatteryLine(char*, FILE*, FILE*);

int main(int argc, char *argv[]) {
    if (argc == 1 || (strcmp(argv[1], COMMAND_HELP_FLAG) == 0)) { // help syntax
        printf("No arguments provided.\n");
        printf("CompactLog is a command line tool that compacts multiple sequential same log lines into single line such as:\n2024-11-20  8:22:29    18424:   32148 AppAssistManager::CheckAndHideWindow:exe=msedge.exe\n2024-11-20  8:22:29    18424:   32148 AppAssistManager::CheckAndHideWindow:exe=msedge.exe\n2024-11-20  8:22:29    18424:   32148 AppAssistManager::CheckAndHideWindow:exe=msedge.exe\nTo:\n2024-11-20  8:22:29    18424:   32148 AppAssistManager::CheckAndHideWindow:exe=msedge.exe * {number of repeated}\n\nThe command syntax is:\n\nCompactLog -i  {input_logfile} -o {output_logfile} \nCompactLog -h  // help syntax\nCompactLog // help syntax\n");
        return 0;
    }
    if (argc != COMMAND_NUM_ARGS ||
        (strcmp(argv[1], COMMAND_INPUT_FLAG) != 0) ||
        (strcmp(argv[3], COMMAND_OUTPUT_FLAG) != 0)) { // check if the command is correct
        printf("The command syntax is:\n\
            \nCompactLog -i  {input_logfile} -o {output_logfile} \
            \nCompactLog -h  // help syntax\
            \nCompactLog // help syntax\n");
        return 0;
    }
    bool res = compactLine(argv[2], argv[4]);

    if (res == false) {
        fprintf(stderr, "Error processing files: %s, %s \n", argv[2], argv[4]);
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
    if(NULL == inputFile || NULL == outputFile){
        fprintf(stderr, "Error opening file: %s: %s \n", inputF, strerror(errno));
        return false;
    }
    char line[MAX_LINE_LENGTH];
    char* prev = NULL;
    int count = 1;

    while (fgets(line, MAX_LINE_LENGTH, inputFile)) {
        // trim off \r and \n
        line[strcspn(line, "\r\n")] = '\0';
        int len = strlen(line);
        if (len < DATE_SIZE) {
            continue; // skip this line if it's too short
        }
        // trim off date
        char no_date[len - DATE_SIZE + 1];
        strncpy(no_date, line + DATE_SIZE, len - DATE_SIZE);

        // check if its a battery line
        if (strlen(no_date) > POSITION_OF_RES) {
            if (no_date[POSITION_OF_RES] == 'r' && no_date[POSITION_OF_RES + 1] == 'e' && no_date[POSITION_OF_RES + 2] == 's' && no_date[POSITION_OF_RES + 3] == '=') {
            // got to battery method
                prev = isBatteryLine(line, outputFile, inputFile);
                // trim line again
                prev[strcspn(prev, "\r\n")] = '\0';
                int len = strlen(prev);
                if (len < DATE_SIZE) {
                    continue; // skip this line if it's too short
                }
                // trim off date
                char no_date[len - DATE_SIZE + 1];
                strncpy(no_date, line + DATE_SIZE, len - DATE_SIZE);
                count = 1;
            }
        }
        if (prev == NULL) {
            prev = strdup(no_date);
        } else if (strcmp(no_date, prev) == 0) {
            count++;
        // } else if (len > DW_CONTROL_POSITION && strncmp(no_date + DW_CONTROL_POSITION, "dwControl=", 10) == 0) {
        // got to dw method
        } else {
            if (count > 1) {
                fprintf(outputFile,"%s * %d\n", prev, count);
            }
            fflush(outputFile);
            prev = strdup(line);
            count = 1;
        }
    }
    free(prev);
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
    bool is7 = false;
    bool is1c = false;
    bool is6 = false;

    char battString[10]; // free later
    int battInd = 0;
    int battMin = 100;
    int battMax = 0;

    do {
        line[strcspn(line, "\r\n")] = '\0';
        int len = strlen(line);
        if (len < DATE_SIZE) {
            continue; // skip this line if it's too short
        }
        // trim off date
        char no_date[len - DATE_SIZE + 1];
        strncpy(no_date, line + DATE_SIZE, len - DATE_SIZE);
        len = strlen(no_date);

        if (no_date[POSITION_OF_RES] == 'r' && no_date[POSITION_OF_RES + 1] == 'e' && no_date[POSITION_OF_RES + 2] == 's' && no_date[POSITION_OF_RES + 3] == '=') {
            if (!is7 && no_date[POSITION_OF_RES_VAL + 2] == '7') {
                is7 = true;
            } else if (!is1c && no_date[POSITION_OF_RES_VAL + 2] == '1' && no_date[POSITION_OF_RES_VAL + 3] == 'c') {
                is1c = true;
            } else if (!is6 && no_date[POSITION_OF_RES_VAL + 2] == '6') {
                is6 = true;
            }

            int i = POSITION_OF_BATTERY_VAL;
            if (is1c == true) {
                // position based if res = 0x7, if 0x1c, add one more to line up
                i = i + 1;
            }
            for (i; i <= len; i++) { // populate battString with the value of battery
                if (i == len || no_date[i] == '\n' || no_date[i] == ' ') {
                    battString[battInd] = '\0'; // null terminate battString
                    int battVal = atoi(battString);
                    if (battMin > battVal) {
                        battMin = battVal; // update min battery value
                    }else if (battMax < battVal) {
                        battMax = battVal; // update max battery value
                    }
                    battString[0] = '\0'; // reset battString
                    battInd = 0; // reset battInd
                    break;
                } else {
                    battString[battInd] = no_date[i]; // append to battString
                    battInd++;
                }
            }
        } else {
            char* resString = "";
            if (is7 && !is1c && !is6) {
                resString = "0x7";
            } else if (is1c && !is7 && !is6) {
                resString = "0x1c";
            } else if (is7 && is1c && !is6) {
                resString = "0x7, 0x1c";
            } else if (is6 && !is7 && !is1c) {
                resString = "0x6";
            } else if (is6 && is7 && !is1c) {
                resString = "0x6, 0x7";
            } else if (is6 && is1c && !is7) {
                resString = "0x6, 0x1c";
            } else if (is6 && is7 && is1c) {
                resString = "0x6, 0x7, 0x1c";
            }

            if (battMax == battMin || battMin == 100) {
                sprintf(battString, "%d", battMax);
            } else if (battMax == 0) {
                sprintf(battString, "%d", battMin);
            } else {
                sprintf(battString, "%d-%d", battMin, battMax);
            }

            fprintf(outputFile, "NotifyManager::ReportStatusAndWait:curl_easy_perform failed res=%s battery=%s\n", resString, battString);
            return line; // return the line as is, no need to write to output file
        }
    } while (fgets(line, MAX_LINE_LENGTH, inputFile));
    return line;
}