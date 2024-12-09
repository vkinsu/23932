#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable : 4996)
struct Node
{
    char* data;
    struct Node* next;
};

void insert(struct Node** head,struct Node** tail, const char* str)
{
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (!newNode)
    {
        fprintf(stderr, "Error!\n");
        return 0;
    }
    int len = strlen(str);
    newNode->data = (char*)malloc(len + 1); 
    newNode->next = NULL;
    strcpy(newNode->data, str);
    if (*head == NULL)
    {
        *head = newNode;
    }
    else
    {
        (*tail)->next = newNode;
    }
    *tail = newNode;
}

void printList(struct Node* head)
{
    struct Node* current = head;
    while (current != NULL)
    {
        printf("%s\n", current->data);
        current = current->next;
    }
}


int main()

{
    struct Node* head = NULL;
    struct Node* tail = NULL;
    char input1[1024];
    while (1)
    {
        fgets(input1, sizeof(input1), stdin);
        if (input1[0] == '.')
        {
            break;
        }

        insert(&head,&tail, input1);
    }

    printf("You entered:\n");
    printList(head);
    struct Node* current = head;
    while (current != NULL)
    {
        head = current->next;
        free(current->data);
        free(current);
        current = head->next;
    }
    free(head);
    return 0;
}
