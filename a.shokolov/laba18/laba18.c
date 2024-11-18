#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void print_permissions(mode_t mode) {
    printf((S_ISDIR(mode)) ? "d" : (S_ISREG(mode)) ? "-" : "?");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> [file2] [file3] ...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++) {
        struct stat file_stat;
        if (stat(argv[i], &file_stat) == -1) {
            perror("stat");
            continue;
        }


        print_permissions(file_stat.st_mode);


        printf(" %ld", (long)file_stat.st_nlink);


        struct passwd *owner_info = getpwuid(file_stat.st_uid);
        if (owner_info != NULL) {
            printf(" %s", owner_info->pw_name);
        } else {
            perror("getpwuid");
            printf(" %d", (int)file_stat.st_uid);
        }


        struct group *group_info = getgrgid(file_stat.st_gid);
        if (group_info != NULL) {
            printf(" %s", group_info->gr_name);
        } else {
            perror("getgrgid");
            printf(" %d", (int)file_stat.st_gid);
        }


        printf(" %ld", (long)file_stat.st_size);


        char time_str[20];
        strftime(time_str, sizeof(time_str), "%b %d %H:%M", localtime(&(file_stat.st_mtime)));
        printf(" %s", time_str);


        printf(" %s\n", argv[i]);
    }

    return 0;
}
