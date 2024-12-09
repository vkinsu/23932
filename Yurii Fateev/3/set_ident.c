#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


void print_user_id(){
    uid_t uid = getuid();
    uid_t euid = geteuid();
    printf("Real: %d\n", uid);
    printf("Effective: %d\n", euid);
}


void open_file(){
    FILE *file;
    file = fopen("test.txt", "r");
    if (file == NULL){
        perror("Error in open file\n");
        return;
    }
    fclose(file);
}


int main(){
    printf("First try:\n");
    print_user_id();
    open_file();
    printf("Second try:\n");
    uid_t uid = getuid();
    setuid(uid);
    print_user_id();
    open_file();
    return 0;
}