#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h> 
#include <sys/un.h>
#include <memory.h> 

static char* msgs[] = { "Hello\n", "i am\n", "a Message\n"};

#define BUFF_SIZE 256
const char* soket_name = "socket";

int main(size_t argc, char** argv)
{
    printf("Client is starting...\n");
    char buffer[BUFF_SIZE];
    int server_sock, client_sock;
    int answer;
    int stat;
    struct sockaddr_un sock_addr, client_addr; 

    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);

    if (client_sock == -1) 
    {
        perror("Client: Failure with creating socket!\n");
        exit(EXIT_FAILURE);
    }

    // Cleaning the structure with trash
    memset((char*)&client_addr, '\0', sizeof(client_addr));
    client_addr.sun_family = AF_UNIX;
    strncmp(client_addr.sun_path, soket_name, strlen(soket_name)-1);    
    unlink(soket_name);  

    if (connect(client_sock, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1)
    {
        perror("Client: Failure with connectint to socket!\n");
        close(client_sock);
        exit(EXIT_FAILURE);
    }
    printf("Client: Client is started.\n");

    for (int i = 0; i < 3; i++)
    {
        if (send(client_sock, msgs[i], strlen(msgs[i]), 0) == -1)
        {
            perror("Client: Error with sending message\n");
            close(client_sock);
            exit(EXIT_FAILURE);
        }
    }

    shutdown(client_sock, SHUT_RD);
    close(client_sock);
    exit(EXIT_SUCCESS);
}
