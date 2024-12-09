#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR_SIZE 256

typedef struct Node {
    char *data;
    struct Node *next;
} Node;


Node* add_to_list(Node *head, const char *str) {
    Node *new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "error memory location.\n");
        exit(1);
    }

    new_node->data = (char*)malloc(strlen(str) + 1);
    if (new_node->data == NULL) {
        fprintf(stderr, "error memory location.\n");
        free(new_node);
        exit(1);
    }

    strcpy(new_node->data, str);
    new_node->next = NULL;

    if (head == NULL) {
        return new_node;
    } else {
        Node *current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
    return head;
}


void free_list(Node *head) {
    Node *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp->data);
        free(temp);
    }
}

void print_list(Node *head) {
    Node *current = head;
    while (current != NULL) {
        printf("%s", current->data);
        current = current->next;
    }
}

int main() {
    Node *head = NULL;
    char buffer[MAX_STR_SIZE];

    printf("Enter strings: \n");
    
    while (1) {
        if (fgets(buffer, MAX_STR_SIZE, stdin) == NULL) {
            fprintf(stderr, "Error reading str.\n");
            break;
        }

        
        if (buffer[0] == '.') {
            break;
        }

        head = add_to_list(head, buffer);
    }

    
    printf("your strings: \n");
    print_list(head);
    free_list(head);

    return 0;
}
