#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Cấu trúc dữ liệu của một phần tử trong danh sách liên kết
struct Node {
    int login;
    char *text_data;
    int text_length;
    int socket_fd;
    struct Node *next;
};

// Danh sách liên kết
struct LinkedList {
    struct Node *head;
};

// Hàm tạo một node mới
struct Node *createNode(int login, const char *text_data, int text_length, int socket_fd) {
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    if (!newNode) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    newNode->login = login;
    newNode->text_data = strdup(text_data);
    newNode->text_length = text_length;
    newNode->socket_fd = socket_fd;
    newNode->next = NULL;

    return newNode;
}

// Hàm thêm một phần tử vào danh sách liên kết
void addNode(struct LinkedList *list, int login, const char *text_data, int text_length, int socket_fd) {
    struct Node *newNode = createNode(login, text_data, text_length, socket_fd);

    if (!list->head) {
        list->head = newNode;
    } else {
        struct Node *current = list->head;
        while (current->next) {
            current = current->next;
        }
        current->next = newNode;
    }
}

// Hàm xóa một phần tử từ danh sách liên kết dựa trên socket_fd
void removeNode(struct LinkedList *list, int socket_fd) {
    if (!list->head) {
        return;
    }

    struct Node *current = list->head;
    struct Node *prev = NULL;

    while (current) {
        if (current->socket_fd == socket_fd) {
            if (prev) {
                prev->next = current->next;
            } else {
                list->head = current->next;
            }
            free(current->text_data);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Hàm tìm kiếm phần tử trong danh sách liên kết dựa trên socket_fd
int searchNode(struct LinkedList *list, int socket_fd) {
    struct Node *current = list->head;

    while (current) {
        if (current->socket_fd == socket_fd) {
            return 1; // Phần tử có socket_fd này đã tồn tại trong danh sách
        }
        current = current->next;
    }

    return 0; // Không tìm thấy phần tử có socket_fd trong danh sách
}

// Hàm thêm văn bản vào cuối dữ liệu của phần tử có socket_fd chỉ định
void appendText(struct LinkedList *list, int socket_fd, const char *additional_text) {
    struct Node *current = list->head;

    while (current) {
        if (current->socket_fd == socket_fd) {
            int old_length = current->text_length;
            int additional_length = strlen(additional_text);

            // Thay đổi kích thước vùng nhớ để chứa văn bản mới
            current->text_data = realloc(current->text_data, old_length + additional_length + 1);
            if (!current->text_data) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
            }

            // Nối văn bản mới vào cuối dữ liệu hiện tại
            strcpy(current->text_data + old_length, additional_text);
            current->text_length += additional_length;

            return;
        }
        current = current->next;
    }
}

// Hàm in danh sách liên kết
void printList(struct LinkedList *list) {
    struct Node *current = list->head;

    while (current) {
        printf("Login: %d, Text Data: %s, Text Length: %d, Socket FD: %d\n",
               current->login, current->text_data, current->text_length, current->socket_fd);
        current = current->next;
    }
}

// Hàm giải phóng bộ nhớ của danh sách liên kết
void freeList(struct LinkedList *list) {
    struct Node *current = list->head;
    struct Node *next;

    while (current) {
        next = current->next;
        free(current->text_data);
        free(current);
        current = next;
    }

    list->head = NULL;
}

int main() {
    // Tạo danh sách liên kết mới
    struct LinkedList list;
    list.head = NULL;
    char *partial_data = NULL;

    // Thêm một số phần tử vào danh sách liên kết
    addNode(&list, 1, partial_data, 5, 10);
    addNode(&list, 2, "World", 5, 20);
    addNode(&list, 3, "12345", 5, 30);

    // In danh sách liên kết
    printf("Initial List:\n");
    printList(&list);

    // Thêm văn bản vào cuối dữ liệu của phần tử có socket_fd 20
    appendText(&list, 20, " Additional Text");
    printf("\nAfter Appending Text:\n");
    printList(&list);

    // Kiểm tra xem phần tử có socket_fd 20 có tồn tại không
    int result = searchNode(&list, 20);
    if (result) {
        printf("\nNode with Socket FD 20 exists in the list.\n");
    } else {
        printf("\nNode with Socket FD 20 does not exist in the list.\n");
    }

    // Xóa phần tử có socket_fd 10 khỏi danh sách liên kết
    removeNode(&list, 10);
    printf("\nAfter Removing Node with Socket FD 10:\n");
    printList(&list);

    // Giải phóng bộ nhớ của danh sách liên kết
    freeList(&list);

    return 0;
}
