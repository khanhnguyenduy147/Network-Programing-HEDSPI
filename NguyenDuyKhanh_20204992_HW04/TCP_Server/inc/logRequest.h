#ifndef LOGREQUEST_H
#define LOGREQUEST_H
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
void logRequest(const char *client_ip, int client_port, const char *result);
#endif