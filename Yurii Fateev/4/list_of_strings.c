#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct node {
    struct node* next;
    char* val;
} node;

#define buff_size 200

int main() {
    char* buff;
    node start, *pref, *curr;
    buff = (char*)malloc((buff_size + 2) * sizeof(char));
    pref = &start;
    start.next = NULL;
    do {
        fgets(buff, buff_size + 1, stdin);
        if (buff[0] == '.') {
            break;
        }
        curr = (node*)malloc(sizeof(node));
        curr->val = (char*)malloc((strlen(buff) + 2) * sizeof(char));
        strcpy(curr->val, buff);
        curr->next = NULL;
        pref->next = curr;
        pref = curr;
    } while (1);
    curr = &start;
    if (curr->next == NULL) {
        printf("\n");
        return 0;
    }
    do {
        curr = curr->next;
        printf("%s", curr->val);
    } while(curr->next);
    return 0;
}
