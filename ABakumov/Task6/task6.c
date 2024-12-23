#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#define TEXT_FILE_NAME "textfile"
#define BUFF_SIZE 10000

typedef struct List_s {
  off_t strOffs;
  off_t strLeng;
  struct List_s* next;
} List;

static void signalHandler(int sig);

int main(int argc, char* argv[])
{

  int fd = open(TEXT_FILE_NAME, O_RDONLY);
  if (fd == -1)
  {
    perror("Cannot open file");
    exit(13);
  }
  lseek(fd, 0L, SEEK_CUR);

  List* head = NULL;

  int count = 0;
  off_t strOffs = 0;
  off_t strLeng = 0;
  char c;

  while (read(fd, &c, 1) == 1)
  {
    if (c == '\n')
    {
      List* node = (List*)malloc(sizeof(List));
      node->strOffs = strOffs;
      node->strLeng = strLeng;
      node->next = head;
      head = node;

      strOffs += strLeng + 1;
      strLeng = 0;
      count++;
    }
    else strLeng++;
  }


  if (strLeng > 0)
  {
    List* node = (List*)malloc(sizeof(List));
    node->strOffs = strOffs;
    node->strLeng = strLeng;
    node->next = head;
    head = node;
    count++;
  }

  signal(SIGALRM, signalHandler);
  alarm(5);

  while (1)
  {
    int i;
    printf("String number: ");
    if (scanf("%d", &i) != 1)
    {
      printf("Incorrect number\n");
      fseek(stdin,0,SEEK_END);
      continue;
    }
    alarm(0);

    if (i == 0) return 0;

    if (count < i)
    {
       printf("Incorrect number (greater than strings count)\n");
       continue;
    }

    List* temp = head;
    for (int n = count - i; n > 0; n--)
      temp = temp->next;
    char buf[BUFF_SIZE];
    lseek(fd, temp->strOffs, SEEK_SET);
    read(fd, buf, temp->strLeng);
    buf[temp->strLeng] = '\0';

    printf("%s\n", buf);
  }

  return 0;
}

static void signalHandler(int sig)
{
    printf("Time out\n");
    char c;
    int fd = open(TEXT_FILE_NAME, O_RDONLY);
    lseek(fd, 0L, SEEK_SET);
    while (read(fd, &c, 1) == 1)
      printf("%c", c);
    exit(0);
}
