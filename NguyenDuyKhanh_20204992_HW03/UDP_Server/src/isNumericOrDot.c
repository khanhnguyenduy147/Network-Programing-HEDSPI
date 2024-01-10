#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/**
 * isNumericOrDot - Check if a string contains only numeric characters or dots.
 *
 * This function examines a given string character by character to determine if it contains
 * only numeric characters (0-9) and dots (.) or not. It returns true if the string consists
 * solely of numeric characters and dots; otherwise, it returns false.
 *
 * @param str The input string to be checked.
 * @return True if the string contains only numeric characters and dots, false otherwise.
 */

bool isNumericOrDot(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!(str[i] == '.' || (str[i] >= '0' && str[i] <= '9'))) {
            return false;
        }
    }
    return true;
}