#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define MAX_PATH_LENGTH 1024
#define SIX_MONTHS (6 * 30 * 24 * 60 * 60)

void print_permissions(mode_t mode)
{
    if (S_ISDIR(mode))
    {
        printf("d");
    }
    else if (S_ISREG(mode))
    {
        printf("-");
    }
    else if (S_ISLNK(mode))
    {
        printf("l");
    }
    else
    {
        printf("?");
    }

    printf("%c", (mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (mode & S_IXUSR) ? 'x' : '-');
    printf("%c", (mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (mode & S_IXGRP) ? 'x' : '-');
    printf("%c", (mode & S_IROTH) ? 'r' : '-');
    printf("%c", (mode & S_IWOTH) ? 'w' : '-');
    printf("%c", (mode & S_IXOTH) ? 'x' : '-');
}

void print_time(time_t mtime)
{
    char time_str[64];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&mtime);

    if ((now - mtime) > SIX_MONTHS || (mtime > now))
    {
        strftime(time_str, sizeof(time_str), "%b %d  %Y", tm_info);
    }
    else
    {
        strftime(time_str, sizeof(time_str), "%b %d %H:%M", tm_info);
    }

    printf(" %-12s", time_str);
}

void print_file_info(const char *file_path)
{
    struct stat file_stat;

    if (lstat(file_path, &file_stat) == -1)
    {
        fprintf(stderr, "Error accessing %s: %s\n", file_path, strerror(errno));
        return;
    }

    print_permissions(file_stat.st_mode);
    printf(" %2ld", file_stat.st_nlink);

    struct passwd *pw = getpwuid(file_stat.st_uid);
    struct group *gr = getgrgid(file_stat.st_gid);

    printf(" %-8s %-8s", pw ? pw->pw_name : "unknown", gr ? gr->gr_name : "unknown");

    if (S_ISREG(file_stat.st_mode))
    {
        printf(" %8ld", file_stat.st_size);
    }
    else if (S_ISDIR(file_stat.st_mode))
    {
        printf(" %8s", "-");
    }
    else if (S_ISLNK(file_stat.st_mode))
    {
        printf(" %8s", "-");
    }
    else
    {
        printf("         ");
    }

    print_time(file_stat.st_mtime);

    const char *file_name = strrchr(file_path, '/');
    file_name = file_name ? file_name + 1 : file_path;
    printf(" %s", file_name);

    if (S_ISLNK(file_stat.st_mode))
    {
        char link_target[MAX_PATH_LENGTH];
        ssize_t len = readlink(file_path, link_target, sizeof(link_target) - 1);
        if (len != -1)
        {
            link_target[len] = '\0';
            printf(" -> %s", link_target);
        }
    }

    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <file1> <file2> ...\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++)
    {
        print_file_info(argv[i]);
    }

    return 0;
}
