#include <stdio.h>
#include <string.h>
#include <time.h>

#ifndef MENU_H
#define MENU_H

int getMenu();
int checkAccount(const char* username);
void logActivity(const char *function, const char *username, const char *result);
void loginAccount(int *login);
void postMessage(int login);
void logoutAccount(int *login);

#endif