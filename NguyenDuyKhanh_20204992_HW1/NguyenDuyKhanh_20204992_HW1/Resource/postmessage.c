#include <stdio.h>
#include <string.h>
#include <time.h>

void flushInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Đọc và loại bỏ các ký tự cho đến khi gặp '\n' hoặc kết thúc tập tin
    }
}

//Function of 2. Post message
void postMessage(int login){
    char message[100];
    printf("Post message: ");
    flushInputBuffer();
    fgets(message, sizeof(message), stdin);

    // Remove newline characters (if any)
    size_t len = strlen(message);
    if (len > 0 && message[len - 1] == '\n') {
        message[len - 1] = '\0'; 
    }

    if(login == 1){
        printf("Successful post\n");
        logActivity("2", message, "+OK");
    }else{
        printf("You have not logged in.\n");
        logActivity("2", message, "-ERR");
    }
}