#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * logRequest - Log a request and its result to a log file.
 *
 * This function takes a request and its corresponding result and writes
 * them to a log file with a specific format. The log file's name is hardcoded
 * as "log_20204992.txt" in this example.
 *
 * @param request The request.
 * @param result  The result of the request.
 */

void logRequest(const char *request, const char *result) {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    FILE *logFile = fopen("log_20204992.txt", "a");
    if (logFile != NULL) {
        fprintf(logFile, "[%02d/%02d/%04d %02d:%02d:%02d]$%s$%s\n",
                timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, request, result);
        fclose(logFile);
    } else {
        perror("Error opening log file");
    }
}