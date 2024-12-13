#include <stdlib.h>
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
#include <poll.h>

#define BUFF_SIZE 256
#define SOCKET_PATH "socket"

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
    char buffer[BUFF_SIZE];
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

        int server_sock, client_sock;
        int answer, len_client;
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
                    
        printf("Server: Server is started.\n");

        struct pollfd clients[10];
        int client_count = 1;

        clients[0].fd = server_sock;    
        clients[0].events = POLLIN;    

        for (int i = 1; i < 10; i++)
        {
            clients[i].fd = -1;
        }

        while (1)
        {
            // Listen activities 
            int activity = poll(clients, client_count, -1);  // Блокируемся до события
            if (activity < 0) 
            {
                perror("Server: poll error\n");
                close(server_sock);
                exit(EXIT_FAILURE);
            }
                        
            // New client
            if (clients[0].revents & POLLIN)
            {
                client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &len_client);
                if (client_sock < 0)
                {
                    perror("Server: accept error\n");
                    close(server_sock);
                    exit(EXIT_FAILURE);
                }

                for (int i = 1; i < 10; i++) {
                if (clients[i].fd == -1) {
                    clients[i].fd = client_sock;
                    clients[i].events = POLLIN;
                    if (i >= client_count) client_count = i + 1;
                    break;
                }
            }

                if (client_count == 10)
                {
                    perror("Server: accept error (too many clients)\n");
                    close(server_sock);
                    exit(EXIT_FAILURE);
                }
            }
                            
            for (int i = 1; i < client_count; i++)
            {
                if (clients[i].fd == -1) continue;

                if (clients[i].revents & POLLIN)
                {
                    memset(buffer,0, BUFF_SIZE);
                    answer = recv(clients[i].fd, buffer, BUFF_SIZE, 0);

                    if (answer <= 0)
                    {
                        printf("Client %d is closed\n", clients[i].fd);
                        close(clients[i].fd);
                        clients[i].fd = -1;  // Удаляем из массива      
                    }
                    else
                    {
                        buffer[answer] = '\0';
                        toupper_string(buffer, strlen(buffer));
                        printf("%s\n", buffer);
                    }
                }
            }
        } 
        
        shutdown(server_sock, SHUT_RD);
        close(server_sock);
        exit(EXIT_SUCCESS);   
    }
    exit(EXIT_SUCCESS);   
}
