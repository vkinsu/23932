#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/select.h>

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

  List* head = NULL;

  int count = 0;
  off_t strOffs = 0;
  off_t strLeng = 0;
  char c;

  struct stat fileInfo;
  if (fstat(fd, &fileInfo) == -1)
  {
    perror("Cannot read file info");
    exit(13);
  }
  size_t size = fileInfo.st_size;

  const char* mapped = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (mapped == MAP_FAILED)
  {
    perror("Failed to map file");
    exit(13);
  }

  for (int i = 0; i < size; i++)
  {
    c = mapped[i];
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
    scanf("%d", &i);
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
    for (int n = 0; n < temp->strLeng; n++)
        printf("%c", mapped[temp->strOffs + n]);
    printf("\n");
  }

  return 0;
}

static void signalHandler(int sig)
{
    printf("Time out\n");
    char c;
    int fd = open(TEXT_FILE_NAME, O_RDONLY);
    struct stat fileInfo;
    if (fstat(fd, &fileInfo) == -1)
    {
      perror("Cannot read file info");
      exit(13);
    }
    size_t size = fileInfo.st_size;

    const char* mapped = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED)
    {
      perror("Failed to map file");
      exit(13);
    }

    for (int i = 0; i < size; i++)
    {
      c = mapped[i];
      printf("%c", c);
    }
    exit(0);
}
