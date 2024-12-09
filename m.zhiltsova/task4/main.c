#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *data;
    struct Node *next;
} Node;

Node* createNode(const char *str) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = (char *)malloc(strlen(str) + 1);
    if (newNode->data == NULL) {
        free(newNode);
        fprintf(stderr, "Memory allocation for string failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(newNode->data, str);
    newNode->next = NULL;
    return newNode;
}

void freeList(Node *head) {
    Node *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp->data); 
        free(temp);      
    }
}

int main() {
    char input[256]; 
    Node *head = NULL; 
    Node *tail = NULL;

    printf("Type lines (for finishing type '.'): \n");

    while (1) {
        fgets(input, sizeof(input), stdin);

        input[strcspn(input, "\n")] = '\0';

        if (input[0] == '.') {
            break;
        }

        Node *newNode = createNode(input);
        if (head == NULL) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    printf("\nLines:\n");
    Node *current = head;
    while (current != NULL) {
        printf("%s\n", current->data);
        current = current->next;
    }

    freeList(head);

    return 0;
}

