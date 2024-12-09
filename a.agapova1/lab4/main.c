#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_SIZE 100

struct Node {
	char *data;
	struct Node *next;
};

struct Node* create_node(const char *str) {
	struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
	if (!new_node) {
		perror("Memory allocation error");
		exit(EXIT_FAILURE);
	}
	new_node->data = (char *)malloc(strlen(str) + 1);
	if (!new_node->data) {
		perror("Memory allocation error");
		exit(EXIT_FAILURE);
	}
	strcpy(new_node->data, str);
	new_node->next = NULL;
	return new_node;
}

void append_node(struct Node **head, const char *str) {
	struct Node *new_node = create_node(str);
	if (*head == NULL) {
		*head = new_node;
	} else {
		struct Node *temp = *head;
		while (temp->next != NULL) {
		temp = temp->next;
	}
	temp->next = new_node;
	}
}

void print_list(struct Node *head) {
	struct Node *temp = head;
	while (temp != NULL) {
		printf("%s", temp->data);
		temp = temp->next;
	}
}

void free_list(struct Node *head) {
	struct Node *temp;
	while (head != NULL) {
		temp = head;
		head = head->next;
		free(temp->data);
		free(temp);
	}
}

int main() {
	struct Node *head = NULL;
	char input[MAX_INPUT_SIZE];

	while (1) {
		printf("Enter a string (enter '.' to stop): ");
		if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
			break;
		}
		if (input[0] == '.') {
			break;
		}
		append_node(&head, input);
		}

	printf("You entered:\n");
	print_list(head);
	free_list(head);
	return 0;
}

