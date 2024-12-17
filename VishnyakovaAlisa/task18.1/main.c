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

void print_file_info(const char* file_path) {
    struct stat file_stat;

    
    if (lstat(file_path, &file_stat) == -1) {
        perror(file_path);
        return;
    }

    
    char file_type;
    if (S_ISDIR(file_stat.st_mode)) {
        file_type = 'd';
    }
    else if (S_ISREG(file_stat.st_mode)) {
        file_type = '-';
    }
    else {
        file_type = '?';
    }

    
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

    
    nlink_t links = file_stat.st_nlink;

    
    struct passwd* pw = getpwuid(file_stat.st_uid);
    struct group* gr = getgrgid(file_stat.st_gid);
    const char* owner = pw ? pw->pw_name : "unknown";
    const char* group = gr ? gr->gr_name : "unknown";

    
    char size[16] = "-";
    if (file_type == '-') {
        snprintf(size, sizeof(size), "%lld", (long long)file_stat.st_size);
    }

    
    char mod_time[20];
    strftime(mod_time, sizeof(mod_time), "%Y-%m-%d %H:%M", localtime(&file_stat.st_mtime));

    
    char* file_name = basename((char*)file_path);

    
    printf("%c%s %3ld %-8s %-8s %8s %s %s\n",
        file_type, permissions, links, owner, group, size, mod_time, file_name);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> [file2 ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        print_file_info(argv[i]);
    }

    return EXIT_SUCCESS;
}