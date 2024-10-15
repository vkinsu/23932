#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *data;
    struct Node *next;
} Node;

Node *head = NULL;
Node *tail = NULL;

void append(char *str) {
    Node *newNode = malloc(sizeof(Node));
    newNode->data = strdup(str);
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
        tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
}

void printList() {
    Node *current = head;
    while (current != NULL) {
        printf("%s\n", current->data);
        current = current->next;
    }
}

void freeList() {
    Node *current = head;
    while (current != NULL) {
        Node *next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
}

int main() {
    char input[1024];

    while (1) {
        fgets(input, 1024, stdin);

        if (input[0] == '.') {
            break;
        }

        input[strcspn(input, "\n")] = 0;
        append(input);
    }

    printList();
    freeList();

    return 0;
}
