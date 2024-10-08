#include <iostream>
#include <cstring>
#include <cstdlib>

struct Node {
    char *data;
    Node *next;
};

class StringList {
public:
    StringList() : head(nullptr) {}

    void insert(const char *str) {
        Node *newNode = (Node *)malloc(sizeof(Node));
        if (!newNode) {
            std::cerr << "Memory Error" << std::endl;
            return;
        }

        newNode->data = (char *)malloc(strlen(str) + 1);
        if (!newNode->data) {
            std::cerr << "Memory Error" << std::endl;
            free(newNode);
            return;
        }

        strcpy(newNode->data, str);
        newNode->next = head;
        head = newNode;
    }

    void display() {
        Node *current = head;
        while (current != nullptr) {
            std::cout << current->data << std::endl;
            current = current->next;
        }
    }

    ~StringList() {
        Node *current = head;
        while (current != nullptr) {
            Node *next = current->next;
            free(current->data);
            free(current);
            current = next;
        }
    }

private:
    Node *head;
};

int main() {
    StringList list;
    char input[100];

    while (true) {
        std::fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        if (input[0] == '.') {
            break;
        }

        list.insert(input);
    }

    list.display();

    return 0;
}

