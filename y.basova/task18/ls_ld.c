#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void print_file_info(const char *path) {
    struct stat file_stat;
    if (lstat(path, &file_stat) == -1) {
        perror("lstat");
        return;
    }

    char file_type = '?';
    if (S_ISDIR(file_stat.st_mode)) {
        file_type = 'd';
    } else if (S_ISREG(file_stat.st_mode)) {
        file_type = '-';
    }

    char permissions[10] = "---------";
    if (file_stat.st_mode & S_IRUSR) permissions[0] = 'r';
    if (file_stat.st_mode & S_IWUSR) permissions[1] = 'w';
    if (file_stat.st_mode & S_IXUSR) permissions[2] = 'x';
    if (file_stat.st_mode & S_IRGRP) permissions[3] = 'r';
    if (file_stat.st_mode & S_IWGRP) permissions[4] = 'w';
    if (file_stat.st_mode & S_IXGRP) permissions[5] = 'x';
    if (file_stat.st_mode & S_IROTH) permissions[6] = 'r';
    if (file_stat.st_mode & S_IWOTH) permissions[7] = 'w';
    if (file_stat.st_mode & S_IXOTH) permissions[8] = 'x';

    int links = file_stat.st_nlink;

    struct passwd *pwd = getpwuid(file_stat.st_uid);
    struct group *grp = getgrgid(file_stat.st_gid);
    const char *owner = pwd ? pwd->pw_name : "unknown";
    const char *group = grp ? grp->gr_name : "unknown";

    char size_str[16];
    if (S_ISREG(file_stat.st_mode)) {
        snprintf(size_str, sizeof(size_str), "%lld", (long long)file_stat.st_size);
    } else {
        snprintf(size_str, sizeof(size_str), "");
    }

    char time_str[20];
    struct tm *tm_info = localtime(&file_stat.st_mtime);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", tm_info);

    const char *file_name = strrchr(path, '/');
    file_name = file_name ? file_name + 1 : path;

    printf("%c%s %2d %-8s %-8s %10s %s %s\n",
           file_type, permissions, links, owner, group, size_str, time_str, file_name);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> [file2 ...]\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        print_file_info(argv[i]);
    }

    return 0;
}

