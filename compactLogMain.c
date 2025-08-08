/* Compact Log main*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "compactlog.h"

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