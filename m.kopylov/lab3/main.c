#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

struct Node
{
    struct Node* next;
    char* str;
};

struct List
{
    struct Node* current;
    struct Node* head;
}*list;

void push_string(char* str, size_t len)
{
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node*));
    new_node->next = NULL;
    new_node->str = (char*)malloc(sizeof(char)*len);
    memcpy(new_node->str, str, len);
    new_node->str[len-1] = '\0';

    if (list == NULL)
    {
        list = (struct List*)malloc(sizeof(struct List*));
        list->current = new_node;
        list->head = new_node;
        return;
    }

    list->current->next = new_node;
    list->current = new_node;
    return;
}

void print_list()
{
    struct Node* curr = list->head;
    while (curr != NULL)
    {
        printf("[%s] ", curr->str);
        curr = curr->next;
    }
    printf("\n");
    return;
}

void free_list()
{
    struct Node* curr = list->head;
    struct Node* to_free = NULL;
    while (curr != NULL)
    {
        to_free = curr;
        curr = curr->next;
        free(to_free->str);
        free(to_free);
    }
    free(list);
    return;
}

int main()
{
    list = NULL;
    char buffer[1024];
    
    while (1)
    {
        fgets(buffer, 1024, stdin);
        if (buffer[0] == '.') break;

        long int len = strlen(buffer);

        push_string(buffer, len);
       
    }

    print_list();
    free_list();
    return 0;
}
