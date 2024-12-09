#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <libgen.h>


void print_file_info(const char *path) {
    struct stat file_stat;
    if (lstat(path, &file_stat) == -1) {
        perror("lstat");
        return;
    }
    // type of the given file:
    char type;
    if (S_ISDIR(file_stat.st_mode)) {
        type = 'd';
    } else if (S_ISREG(file_stat.st_mode)) {
        type = '-';
    } else {
        type = '?';
    }
    // possible rights:
    char permissions[10];
    permissions[0] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';
    permissions[3] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';
    permissions[6] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';
    permissions[9] = '\0';
    // count of links:
    int links = file_stat.st_nlink;
    // the name of a master:
    struct passwd *pwd = getpwuid(file_stat.st_uid);
    struct group *grp = getgrgid(file_stat.st_gid);
    const char *owner = pwd ? pwd->pw_name : "unknown";
    const char *group = grp ? grp->gr_name : "unknown";
    // files's size (if it's usual):
    char size[16] = "-";
    if (S_ISREG(file_stat.st_mode)) {
        snprintf(size, sizeof(size), "%lld", (long long) file_stat.st_size);
    }
    // modification date:
    char mod_time[20];
    struct tm *timeinfo = localtime(&file_stat.st_mtime);
    strftime(mod_time, sizeof(mod_time), "%Y-%m-%d %H:%M", timeinfo);
    // file's name:
    char *base_name = basename((char *)path);
    // main printing:
    printf("%c%s %3d %-8s %-8s %8s %s %s\n",
           type, permissions, links, owner, group, size, mod_time, base_name);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> <file2> ...\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    for (int i = 1; i < argc; i++) {
        print_file_info(argv[i]);
    }
    return 0;
}