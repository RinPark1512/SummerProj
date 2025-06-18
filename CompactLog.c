#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>

void compactLine(char*, char*);

int main(int argc, char *argv[]) {
    if (argc == 1 || (strcmp(argv[1], "-h") == 0)) { // help syntax
        printf("No arguments provided.\n");
        printf("CompactLog is a command line tool that compacts multiple sequential same log lines into single line such as:\n2024-11-20  8:22:29    18424:   32148 AppAssistManager::CheckAndHideWindow:exe=msedge.exe\n2024-11-20  8:22:29    18424:   32148 AppAssistManager::CheckAndHideWindow:exe=msedge.exe\n2024-11-20  8:22:29    18424:   32148 AppAssistManager::CheckAndHideWindow:exe=msedge.exe\nTo:\n2024-11-20  8:22:29    18424:   32148 AppAssistManager::CheckAndHideWindow:exe=msedge.exe * {number of repeated}\n\nThe command syntax is:\n\nCompactLog -i  {input_logfile} -o {output_logfile} \nCompactLog -h  // help syntax\nCompactLog // help syntax\n");
        return 0;
    }
    if (argc != 5){
        printf("The command syntax is:\n\nCompactLog -i  {input_logfile} -o {output_logfile} \nCompactLog -h  // help syntax\nCompactLog // help syntax\n");
        return 0;
    }
//    0          1    2              3  4
//    CompactLog -i  {input_logfile} -o {output_logfile} 
    compactLine(argv[2], argv[4]);
    // file io
    // open file, load into buffer line by line
    // compare line with the previous
    // if same, count++ and keep going
    // diff, print line + count into out file
    printf("Task Complete! Come Again!\n");


    return 0;
}

void compactLine(char* inputF, char* outputF){
    FILE* inputFile = fopen(inputF,"r");;
    FILE* outputFile = fopen(outputF, "w");
    if(NULL == inputFile || NULL == outputFile){
        fprintf(stderr, "Error opening file: %s: %s \n", inputF, strerror(errno));
        return;
    }
    char line[200];
    char* prev = NULL;
    int count = 1;
    while (fgets(line, 200, inputFile)) {
        // printf("line: %s\n", line);
        line[strcspn(line, "\r\n")] = '\0';
        if (prev == NULL){
            prev = strdup(line);
        }else if(strcmp(line, prev) == 0){
            count++;
        } else{
            // printf("count: %d\n", count);
            fprintf(outputFile,"%s * %d\n", prev, count);
            fflush(outputFile);
            prev = strdup(line);
            count = 1;
        }
    }
    free(prev);
    fclose(inputFile);
    fclose(outputFile);
    return;
}
