#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("No arguments provided.\n");
    exit(13);
  }

  for (int i = 1; i < argc; i++)
  {
    struct stat file_stat;
    if (stat(argv[i], &file_stat) == -1)
    {
      perror("Cannot read file stat");
      exit(13);
    }

    mode_t mode = file_stat.st_mode;

    // Print file type
    if      (S_ISDIR(mode)) printf("d");
    else if (S_ISREG(mode)) printf("-");
    else                    printf("?");

    // Print permissions
    if (mode & S_IRUSR) printf("r"); else printf("-");
    if (mode & S_IWUSR) printf("w"); else printf("-");
    if (mode & S_IXUSR) printf("x"); else printf("-");
    if (mode & S_IRGRP) printf("r"); else printf("-");
    if (mode & S_IWGRP) printf("w"); else printf("-");
    if (mode & S_IXGRP) printf("x"); else printf("-");
    if (mode & S_IROTH) printf("r"); else printf("-");
    if (mode & S_IWOTH) printf("w"); else printf("-");
    if (mode & S_IXOTH) printf("x"); else printf("-");

    // Print user name and group name
    struct passwd *pw = getpwuid(file_stat.st_uid);
    struct group *gr = getgrgid(file_stat.st_gid);

    if (pw != NULL && gr != NULL) printf(" %-8s %-8s", pw->pw_name, gr->gr_name);
    else                          printf(" %-8s %-8s", "null", "null");

    if (S_ISREG(file_stat.st_mode)) printf(" %8ld", file_stat.st_size);
    else                            printf("         ");

    char time_str[20];
    strftime(time_str, sizeof(time_str), "%b %d %H:%M", localtime(&(file_stat.st_mtime)));
    printf(" %s", time_str);

    int slash_i = strlen(argv[i]) - 1;
    while(argv[i][slash_i] != '/' && slash_i > 0) slash_i--;
    if (argv[i][slash_i] == '/') slash_i++;

    printf(" %s \n", argv[i] + slash_i);
  }

  return 0;
}
