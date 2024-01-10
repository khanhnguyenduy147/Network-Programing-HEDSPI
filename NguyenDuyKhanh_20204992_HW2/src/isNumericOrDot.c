#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Hàm kiểm tra xem chuỗi chỉ chứa số hoặc dấu chấm
bool isNumericOrDot(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!(str[i] == '.' || (str[i] >= '0' && str[i] <= '9'))) {
            return false;
        }
    }
    return true;
}