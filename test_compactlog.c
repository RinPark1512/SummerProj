/* Unit tests for compactlog.c */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "compactlog.h"

void test_checkRes() {
    char line[] = "2024-11-20  8:22:29    18424:   32148 NotifyManager::ReportStatusAndWait:curl_easy_perform failed res=0x1c battery=100";
    assert(checkRes(line) == true);

    char badLine[] = "No battery info here";
    assert(checkRes(badLine) == false);
}

void test_storeRes() {
    char line[] = "....res=0x1c battery=90";
    char* resArr[3] = {NULL, NULL, NULL};
    storeRes(line, resArr);
    assert(resArr[1] != NULL && strcmp(resArr[1], "0x1c") == 0);
    assert(resArr[0] == NULL && resArr[2] == NULL);
    for (int i = 0; i < 3; i++) if (resArr[i]) free(resArr[i]);
}

void test_makeResString() {
    char* resArr[3] = {strdup("0x7"), NULL, NULL};
    char* result = makeResString(resArr);
    assert(strcmp(result, "0x7") == 0);
    free(result);
    free(resArr[0]);
}

void test_makeBattString() {
    char* single = makeBattString(85, 85);
    assert(strcmp(single, "85") == 0);
    free(single);

    char* range = makeBattString(80, 90);
    assert(strcmp(range, "80-90") == 0);
    free(range);
}

void test_populateBatt() {
    char line[] = "NotifyManager::ReportStatusAndWait:curl_easy_perform failed res=0x1c battery=95";
    int batt = populateBatt(line);
    assert(batt == 95);
}

void run_all_tests() {
    test_checkRes();
    test_storeRes();
    test_makeResString();
    test_makeBattString();
    test_populateBatt();
    printf("All tests passed!\n");
}

int main() {
    run_all_tests();
    return 0;
}
