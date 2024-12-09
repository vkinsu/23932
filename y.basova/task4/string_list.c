#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char *data;
    struct Node *next;
};

int main() {
    const int MAX_STRING_SIZE = 100;
    
    struct Node *head = NULL;
    struct Node *tail = NULL;

    char input_string[MAX_STRING_SIZE];
    
    while (1) {
        printf("type some text (type . to stop): ");
        fgets(input_string, MAX_STRING_SIZE, stdin);

        if (input_string[0] == '.') {
            break;
        }

        input_string[strcspn(input_string, "\n")] = '\0';

        struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
        if (new_node == NULL) {
            printf("memory error\n");
            exit(1);
        }

        new_node->data = (char *)malloc(strlen(input_string) + 1);
        if (new_node->data == NULL) {
            printf("memory error\n");
            free(new_node);
            exit(1);
        }

        strcpy(new_node->data, input_string);

        new_node->next = NULL;
        if (head == NULL) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }
    }

    printf("\ntyped text:\n");
    struct Node *current = head;
    while (current != NULL) {
        printf("%s\n", current->data);
        current = current->next;
    }

    current = head;
    while (current != NULL) {
        struct Node *next = current->next;
        free(current->data);
        free(current);
        current = next;
    }

    return 0;
}


