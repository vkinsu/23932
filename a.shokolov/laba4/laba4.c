#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Node_s {
    char* string;
    struct Node_s* next;
} Node;

Node* first, * tail;

void init() {
    first = malloc(sizeof(Node));
    first->string = NULL;
    first->next = NULL;
    tail = first;
}

void push(char* string) {
    unsigned long len = strlen(string) + 1;
    char* copyPtr = calloc(len, sizeof(char));
    strcpy(copyPtr, string);
    tail->string = copyPtr;
    tail->next = calloc(1, sizeof(Node));
    tail = tail->next;
}

void printList() {
    Node* ptr = first;
    while (ptr != NULL) {
        if (ptr->string) {
            printf("%s\n", ptr->string);
        }
        ptr = ptr->next;
    }
}

int main() {
    char Inbuf[256] = { 0 };

    init();

    while (fgets(Inbuf, 256, stdin) != NULL) {
        if (Inbuf[0] == '.') {
            printList();
            return 0;
        }

        char* lineEnd = strchr(Inbuf, '\n');
        if (lineEnd == NULL) {
            size_t bufcnt = 0;
            size_t bufsz = 256;
            char* buf = malloc(256);

            memcpy(buf, Inbuf, 256);
            bufcnt += 256 - 1;

            while (fgets(Inbuf, 256, stdin) != NULL) {
                bufsz += 256;
                buf = realloc(buf, bufsz);

                memcpy(buf + bufcnt, Inbuf, 256);
                bufcnt += 256 - 1;

                lineEnd = strchr(buf, '\n');
                if (lineEnd) {
                    *lineEnd = '\0';
                    push(buf);
                    free(buf);
                    break;
                }
            }
        }
        else {
            *lineEnd = '\0';
            push(Inbuf);
        }
    }

    return 0;
}