#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdlib.h>

#define MSG_LEN 30

int main(int argc, char *argv[]) {

    pid_t ppid = getpid();

    int fd[2];  //Pointers to the descriptors will be placed here
                //fd[0] - to read end
                //fd[1] - to write end
                //(They are both bidirectional though)

    const char msg_out[MSG_LEN] = "How tO WRitE A cOdE";

    if(pipe(fd) == -1) {
        perror("Pipe");
        exit(-1);
    }
    printf("[%ld] A pipe was created.\n", ppid);
    pid_t pid = fork();
    
    if(pid > 0) {  //parent
        printf("[%ld] A subprocess with pid %ld was created.\n", ppid, pid);
        close(fd[0]);
        if(write(fd[1], msg_out, MSG_LEN) == -1) {  //stores msg_out to the pipe fd
            perror("Parent writes");
            exit(-1);
        }
        printf("[%ld] A message \"%s\" was send by the pipe.\n[%ld] Waiting for child finishing a process...\n",
                ppid,           msg_out,                        ppid);
        close(fd[1]);
        if(wait(NULL) == -1) { //parent is waiting while child ends
            perror("Parent waits");
            exit(-1);
        }
        printf("[%ld] Child finished the process.\n", ppid);
    }
    else if(pid == 0) { //child
        pid_t cpid = getpid();
        close(fd[1]);
        char msg_in[MSG_LEN];
        if(read(fd[0], msg_in, MSG_LEN) == -1){     //loads a message to the msg_in from the pipe fd
            perror("Child writes");
            exit(-1);
        }
        printf("[%ld] The child received a message \"%s\".\n", cpid, msg_in);
        char *ptr;
        for(ptr = msg_in; *ptr; ptr++){
            *ptr = toupper(*ptr);
        }
        printf("[%ld] The message was uppercased: \"%s\".\n", cpid, msg_in);
        close(fd[0]);
    }
    else {   //if fork doesn't work
        perror("Fork");
        exit(-1);
    }

    return 0;
}