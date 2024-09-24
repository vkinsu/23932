#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    	char *data;
    	struct Node *next;
} Node;

void insert(Node **head, const char *str) {
    	Node *new_node = (Node *)malloc(sizeof(Node));
    	if (!new_node) {
        	exit(EXIT_FAILURE);
    	}

    	new_node->data = (char *)malloc(strlen(str) + 1);
    	if (!new_node->data) {
        	exit(EXIT_FAILURE);
    	}

    	strcpy(new_node->data, str);
    	new_node->next = *head;
    	*head = new_node;
}

void printList(Node *head) {
    	Node *current = head;
    	while (current != NULL) {
        	printf("%s", current->data);
        	current = current->next;
    	}
}

void freeList(Node *head) {
    	Node *current = head;
    	while (current != NULL) {
        	Node *temp = current;
        	current = current->next;
        	free(temp->data);
        	free(temp);
    	}
}

int main() {
	Node *head = NULL;
    	char buffer[256];

    	printf("Enter the lines (enter '.' to end):\n");
    	while (1) {
        	fgets(buffer, sizeof(buffer), stdin);

        	if (buffer[0] == '.') {
            	break;
        	}

        	insert(&head, buffer);
    	}

    	printf("\nThe entered lines:\n");
    	printList(head);

    	freeList(head);

    	return 0;
}
