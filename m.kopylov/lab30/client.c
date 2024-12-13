#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h> 
#include <sys/un.h>
#include <memory.h> 

#define BUFF_SIZE 256
#define SOCKET_PATH "socket"

int main(size_t argc, char** argv)
{
        int pid = fork();
        setbuf(stdin, NULL); 
        printf("Client is starting...\n");
        char buffer[BUFF_SIZE];
        int server_sock, client_sock;
        int answer;

        struct sockaddr_un server_addr; 

        client_sock = socket(AF_UNIX, SOCK_STREAM, 0);

        if (client_sock == -1) 
        {
            perror("Client: Failure with creating socket!\n");
            exit(EXIT_FAILURE);
        }

        // Cleaning the structure with trash
        memset((char*)&server_addr, 0, sizeof(struct sockaddr_un));
        server_addr.sun_family = AF_UNIX;
        strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path)-1);    

        if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        {
            perror("Client: Failure with connectint to socket!\n");
            close(client_sock);
            exit(EXIT_FAILURE);
        }
        printf("Client: Client is started.\n");
        memset(buffer, 0, BUFF_SIZE);

        sprintf(buffer, "Message from client pid: %d", pid);
            
        send(client_sock, buffer, strlen(buffer), 0);
    
        shutdown(client_sock, SHUT_RD);
        close(client_sock);
        exit(EXIT_SUCCESS);
}
