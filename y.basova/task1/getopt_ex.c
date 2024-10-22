#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>

void print_user_group_ids() {
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();
    gid_t real_gid = getgid();
    gid_t effective_gid = getegid();

    printf("Real UID: %d, Effective UID: %d\n", real_uid, effective_uid);
    printf("Real GID: %d, Effective GID: %d\n", real_gid, effective_gid);
}

void print_process_ids() {
    pid_t pid = getpid();
    pid_t ppid = getppid();
    pid_t pgid = getpgid(pid);

    printf("Process ID: %d, Parent Process ID: %d, Process Group ID: %d\n", pid, ppid, pgid);
}

void print_ulimit() {
    struct rlimit rl;
    if (getrlimit(RLIMIT_FSIZE, &rl) == 0) {
        printf("Ulimit (file size): %ld\n", rl.rlim_cur);
    } else {
        perror("getrlimit");
    }
}

void set_ulimit(const char *new_ulimit) {
    long new_limit = strtol(new_ulimit, NULL, 10);
    struct rlimit rl;
    rl.rlim_cur = new_limit;
    rl.rlim_max = new_limit;

    if (setrlimit(RLIMIT_FSIZE, &rl) == 0) {
        printf("Ulimit set to: %ld\n", new_limit);
    } else {
        perror("setrlimit");
    }
}

void print_core_file_size() {
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == 0) {
        printf("Core file size: %ld\n", rl.rlim_cur);
    } else {
        perror("getrlimit");
    }
}

void set_core_file_size(const char *size) {
    long new_size = strtol(size, NULL, 10);
    struct rlimit rl;
    rl.rlim_cur = new_size;
    rl.rlim_max = new_size;

    if (setrlimit(RLIMIT_CORE, &rl) == 0) {
        printf("Core file size set to: %ld\n", new_size);
    } else {
        perror("setrlimit");
    }
}

void print_current_directory() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd");
    }
}

void print_environment_variables() {
    extern char **environ;
    for (char **env = environ; *env != 0; env++) {
        char *thisEnv = *env;
        printf("%s\n", thisEnv);
    }
}

void set_environment_variable(const char *name_value) {
    char *name = strdup(name_value);
    char *value = strchr(name, '=');
    if (value != NULL) {
        *value = '\0';
        value++;
        if (setenv(name, value, 1) == 0) {
            printf("Environment variable set: %s=%s\n", name, value);
        } else {
            perror("setenv");
        }
    } else {
        fprintf(stderr, "Invalid format for environment variable: %s\n", name_value);
    }
    free(name);
}

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "ispucC:dU:vV:")) != -1) {
        switch (opt) {
            case 'i':
                print_user_group_ids();
                break;
            case 's':
                if (setpgid(0, 0) == 0) {
                    printf("Process became the group leader.\n");
                } else {
                    perror("setpgid");
                }
                break;
            case 'p':
                print_process_ids();
                break;
            case 'u':
                print_ulimit();
                break;
            case 'U':
                set_ulimit(optarg);
                break;
            case 'c':
                print_core_file_size();
                break;
            case 'C':
                set_core_file_size(optarg);
                break;
            case 'd':
                print_current_directory();
                break;
            case 'v':
                print_environment_variables();
                break;
            case 'V':
                set_environment_variable(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-i] [-s] [-p] [-u] [-U new_ulimit] [-c] [-C size] [-d] [-v] [-V name=value]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    return 0;
}

