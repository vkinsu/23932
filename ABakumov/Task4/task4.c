#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1000

typedef struct MyList_s {
	char* s;
	struct MyList_s* next;
} MyList;

void rec_list_print(MyList* list)
{
	if (list->next == NULL)
	{
		printf("%s", list->s);
		return;
	}
	rec_list_print(list->next);
	printf("%s", list->s);
}

main(int argc, char *argv[])
{
	char line[BUFFER_SIZE];
	MyList* head = NULL;

	while (1)
	{
		fgets(line, BUFFER_SIZE, stdin);
		if (strcmp(line, ".\n") == 0)
			break;
		MyList* temp = (MyList*)malloc(sizeof(MyList));
		temp->s = (char*)malloc(sizeof(char) * strlen(line) + 1);
		strcpy(temp->s, line);
		temp->next = head;
		head = temp;
	}

	rec_list_print(head);

	return 0;
}
