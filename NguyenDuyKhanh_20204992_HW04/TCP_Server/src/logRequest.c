#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * logRequest - Log a request and its result to a log file.
 *
 * This function logs a request and its corresponding result to a log file. It
 * includes a timestamp, client IP, client port, and the result in a specific format.
 *
 * @param client_ip   The IP address of the client.
 * @param client_port The port of the client.
 * @param result      The result of the request to log.
 */ 

void logRequest(const char *client_ip, int client_port, const char *result) {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    FILE *logFile = fopen("log_20204992.txt", "a");
    if (logFile != NULL) {
        fprintf(logFile, "[%02d/%02d/%04d %02d:%02d:%02d]$%s:%d$%s\n",
                timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, client_ip, client_port, result);
        fclose(logFile);
    } else {
        perror("Error opening log file");
    }
}