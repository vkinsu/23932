#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
//#include <io.h>


char** split(char* buff, int size, int* res_size) {
    char* word = NULL;
    int cnt = 0, curr = 0;
    char** res = (char**)malloc((size / 2 + 1) * sizeof(char*));
    printf("String: %s\n", buff);
    for (int i = 0; i < size; i++) {
        if (buff[i] == '\n' || buff[i] == 0) {
            if (curr != 0) {
                word[curr++] = 0;
                word = (char*)realloc(word, curr * sizeof(char));
                res[cnt++] = word;
                curr = 0;
                word = (char*)malloc(size * sizeof(char));
            }
        }
        else {
            word[curr++] = buff[i];
        }
    }
    printf("Check %d\n", cnt);
    if (curr != 0) {
        word[curr++] = 0;
        word = (char*)realloc(word, curr * sizeof(char));
        res[cnt++] = word;
    }
    res = (char**)realloc(res, cnt * sizeof(char*));
    if (res_size) *res_size = cnt;
    return res;
}


int main() {
    printf("Start\n");
    int fd = open("test2.txt", O_RDWR);
    printf("fd: %d\n", fd);
    int size, readed = 0;
    size = lseek(fd, 0, SEEK_END);
    printf("Size: %d\n", size);
    char* buff = (char*)malloc((size + 1) * sizeof(char));
    buff[size] = 0;
    printf("Buffer: %s\n", buff);
    readed = read(fd, buff, size);
    printf("Readed: %d\n", readed);
    printf("Entered: %s\n", buff);
    int cnt_lines;
    printf("Start split\n");
    char** lines = split(buff, size + 1, &cnt_lines);
    printf("End split\n");
    int num;
    printf("%d\n", cnt_lines);
    printf("Enter number of string that you want to read\n");
    do {
        scanf("%d", &num);
        if (num == 0) return 0;
        if (num < 0 || num > cnt_lines) {
            printf("Incorrect number of string!\n");
            return 0;
        }
        num--;
        printf("%s\n", lines[num]);
    } while(1);
    close(fd);
    for (int i = 0; i < cnt_lines; i++) {
        free(lines[i]);
    }
    free(lines);
    free(buff);
    return 0;
}