#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h> 
#include <sys/un.h>
#include <memory.h> 
#include <ctype.h>
#include <signal.h>
#include <errno.h>

#define BUFF_SIZE 256
#define SOCKET_PATH "socket"

int server_sock, client_sock;
int answer, len_client;
char buffer[BUFF_SIZE];

void toupper_string(char* buff, int len)
{
    int i;
    for (int i = 0; i < len; i++)
    {
        buff[i] = toupper(buff[i]);
    }
}

int main(size_t argc, char** argv)
{
  
    setbuf(stdout, NULL); 
    pid_t proc = fork();
    
    if (proc == 0)
    {
        if (access(SOCKET_PATH, F_OK) != 0) 
        {
            perror("Server: Socket file not ready\n");
            sleep(1);
        }  
        printf("Server: Starting client from child process...\n");
        char* argv[] = {"./client", NULL};
        int val = execvp("./client", argv);
        if (val == -1)
        {
            perror("Server: Something went wrong in the child process!\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (proc == -1)
    {
        printf("Server: Something went wrong!\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Server: Server is starting...\n");

      
        struct sockaddr_un server_addr, client_addr; 

        len_client = sizeof(client_addr);
        server_sock = socket(AF_UNIX, SOCK_STREAM, 0);

        if (server_sock == -1) 
        {
            perror("Server: Failure with creating socket!\n");
            exit(EXIT_FAILURE);
        }

        // Cleaning the structure with trash
        memset((char*)&server_addr, '\0', sizeof(server_addr));
        server_addr.sun_family = AF_UNIX;
        strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path)-1);  
        unlink(SOCKET_PATH);  

        if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        {
            perror("Server: Failure with bind socket!\n");
            close(server_sock);
            exit(EXIT_FAILURE);
        }

        if (listen(server_sock, 1) == -1)
        {
            perror("Server: Failure with listening socket!\n");
            close(server_sock);
            exit(EXIT_FAILURE);
        }

        fcntl(server_sock, F_SETFL, O_NONBLOCK);
                    
        printf("Server: Server is started.\n");

        while (1)
        {
            client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &len_client);
            if (client_sock < 0)
            {
                if (errno == EAGAIN ||  errno == EWOULDBLOCK)
                {
                    usleep(1000);
                    continue;
                }
                else
                {
                    perror("Server: accept error\n");
                    close(server_sock);
                    exit(EXIT_FAILURE);
                }
            } 

            printf("Server: Client %d is connected\n", client_sock);

            while (1)
            {
                memset(buffer, 0, BUFF_SIZE);
                int bytes_received = recv(client_sock, buffer, BUFF_SIZE, 0);
                if (bytes_received > 0)
                {
                    toupper_string(buffer, bytes_received);
                    printf("Server: Received: %s\n", buffer);
                } 
                else if (bytes_received == 0) 
                {
                    printf("Server: Client disconnected\n");
                    close(client_sock);
                    break;
                } 
                else if (errno == EAGAIN || errno == EWOULDBLOCK) 
                {
                    //usleep(1000); 
                    continue;
                } 
                else 
                {
                    perror("Server: recv failed\n");
                    close(client_sock);
                    break;
                }
            }
        } 
        
        shutdown(server_sock, SHUT_RD);
        close(server_sock);
        exit(EXIT_SUCCESS);   
    }
    exit(EXIT_SUCCESS);   
}
