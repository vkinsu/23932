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

#define BUFF_SIZE 256
const char* soket_name = "socket";

void toupper_string(char* buff, int len)
{
    for (int i = 0; i < len; i++)
    {
        buff[i] = toupper(buff[i]);
    }
}

int main(size_t argc, char** argv)
{
    char buffer[BUFF_SIZE];
    pid_t proc = fork();

    switch (proc)
    {
    case 0:
        {
            printf("Server: Starting client from child process...\n");

            char* argv[] = {"./client", NULL};
            int val = execvp("./client", argv);
            if (val == -1)
            {
                perror("Server: Something went wrong in the child process!\n");
                exit(EXIT_FAILURE);
            }
            break;
        }
    case -1: 
        {
            printf("Server: Something went wrong!\n");
            exit(EXIT_FAILURE);
            break;
        }
    default:
        {
            printf("Server: Server is starting...\n");
            

            int server_sock, client_sock;
            int answer;
            int stat;
            struct sockaddr_un sock_addr, client_addr; 

            server_sock = socket(AF_UNIX, SOCK_STREAM, 0);

            if (server_sock == -1) 
            {
                perror("Server: Failure with creating socket!\n");
                exit(EXIT_FAILURE);
            }

            // Cleaning the structure with trash
            memset((char*)&sock_addr, '\0', sizeof(sock_addr));
            sock_addr.sun_family = AF_UNIX;
            strncmp(sock_addr.sun_path, soket_name, strlen(soket_name)-1);      
            unlink(soket_name);  

            if (bind(server_sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) == -1)
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
            int len = 0;
            int msgs = 1;
            while (1)
            {
                answer = sizeof(client_addr);
                client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &answer);
                answer = recv(client_sock, buffer, BUFF_SIZE, 0); 
                if (answer <= 0)
                {
                    printf("Server: Server is stopping\n");
                    
                    break;
                }

                len = strlen(buffer);

                toupper_string(buffer, len);
                printf("%s", buffer);
                
              
                shutdown(server_sock, SHUT_RD);
                close(server_sock);
                break;
            }
        }
    }

    exit(EXIT_SUCCESS);
}
