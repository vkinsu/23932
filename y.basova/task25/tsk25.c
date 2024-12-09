#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 256

int main() {
    int pipefd[2];
    pid_t cpid;
    char buf[BUFFER_SIZE];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {    
        close(pipefd[1]);  

        read(pipefd[0], buf, BUFFER_SIZE);

        for (int i = 0; i < strlen(buf); i++) {
            buf[i] = toupper(buf[i]);
        }

        printf("Converted text: %s\n", buf);

        close(pipefd[0]);
        _exit(EXIT_SUCCESS);
    } else {           
        close(pipefd[0]);  

        char *text = "Hello, World! This is a test.";
        write(pipefd[1], text, strlen(text) + 1);

        close(pipefd[1]);
        wait(NULL);
        exit(EXIT_SUCCESS);
    }

    return 0;
}

