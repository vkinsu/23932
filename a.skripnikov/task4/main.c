#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 1000

typedef struct list
{
    char *str;
    struct list *next;
} list;

list *append(list *latest_elem, char *current_string)
{
    latest_elem->str = (char *)malloc(strlen(current_string) + 1);
    strcpy(latest_elem->str, current_string);
    latest_elem->next = NULL;
    return latest_elem;
}

void print(list *arr)
{
    list *current = arr;
    while (current != NULL)
    {
        printf("%s\n", current->str);
        current = current->next;
    }
}

void freeList(list *arr)
{
    list *current = arr;
    list *next;
    while (current != NULL)
    {
        next = current->next;
        free(current->str);
        free(current);
        current = next;
    }
}

int main()
{
    list *arr = NULL;
    char tmp[SIZE];
    list *latest_elem = NULL;

    printf("Enter strings (start with '.' to stop):\n");

    while (fgets(tmp, SIZE, stdin) && (tmp[0] != '.'))
    {
        tmp[strcspn(tmp, "\n")] = '\0';
        list *new_elem = (list *)malloc(sizeof(list));

        if (arr == NULL)
        {
            arr = new_elem;
        }
        else
        {
            latest_elem->next = new_elem;
        }

        latest_elem = append(new_elem, tmp);
    }

    printf("List of strings:\n");
    print(arr);
    freeList(arr);

    return 0;
}