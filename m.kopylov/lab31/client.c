#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h> 
#include <sys/un.h>
#include <memory.h> 
#include <pthread.h>

#define BUFF_SIZE 256
#define SOCKET_PATH "socket"

int client_sock;

void* send_message(void* param)
{
    printf("Client: Thread is started\n");
    int thread_id = *(int*)param;
    char buffer_t[BUFF_SIZE];
    memset(buffer_t, 0, BUFF_SIZE);
    sprintf(buffer_t, "Message from client's thread %d\n", thread_id);
    send(client_sock, buffer_t, strlen(buffer_t), 0);
}

int main(size_t argc, char** argv)
{
        fork();
        setbuf(stdin, NULL); 
        printf("Client is starting...\n");
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
        pthread_t threads[5];
        int number[5];
        for (int i = 0; i < 5; i++)
        {
            number[i] = i;
            pthread_create(&threads[i], NULL, send_message, &number[i]);

            pthread_detach(threads[i]);
        }            
        
        sleep(2);
        shutdown(client_sock, SHUT_RD);
        close(client_sock);
        exit(EXIT_SUCCESS);
}
