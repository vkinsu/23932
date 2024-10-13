#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <string.h>

extern char **environ;

void print_usage() {
    printf("Usage: program_name [options]\n");
    printf("Options:\n");
    printf("  -i   Display user and group IDs\n");
    printf("  -s   Set the process group ID\n");
    printf("  -p   Display process IDs\n");
    printf("  -u   Display the file size limit\n");
    printf("  -U   Set the file size limit\n");
    printf("  -c   Display the core file size limit\n");
    printf("  -C   Set the core file size limit\n");
    printf("  -d   Display the current directory\n");
    printf("  -v   Display environment variables\n");
    printf("  -V   Set an environment variable\n");
}

void display_user_info() {
    printf("uid: %d, euid: %d, gid: %d, egid: %d\n",
           getuid(), geteuid(), getgid(), getegid());
}

void set_process_group() {
    if (-1 == setpgid(0, 0)) {
        perror("failed to set the group leader process");
    } else {
        printf("the group leader process has been set successfully\n");
    }
}

void display_process_info() {
    printf("pid: %d, ppid: %d, pgrp: %d\n",
           getpid(), getppid(), getpgrp());
}

void display_file_size_limit() {
    struct rlimit rlp;
    if (-1 == getrlimit(RLIMIT_FSIZE, &rlp)) {
        perror("failed to get ulimit");
    } else {
        printf("ulimit value: %lu\n", rlp.rlim_max);
    }
}

void set_file_size_limit(char *arg) {
    long new_ulimit = strtol(arg, NULL, 10);

    if (0 == new_ulimit) {
        perror("invalid argument for the -U option");
        return;
    }

    struct rlimit rlp;
    if (-1 == getrlimit(RLIMIT_FSIZE, &rlp)) {
        perror("failed to get the ulimit value");
        return;
    }
    rlp.rlim_cur = new_ulimit;
    if (-1 == setrlimit(RLIMIT_FSIZE, &rlp)) {
        perror("failed to set the ulimit value");
    } else {
        printf("the ulimit value has been set successfully\n");
    }
}

void display_core_file_size_limit() {
    struct rlimit rlp;
    if (-1 == getrlimit(RLIMIT_CORE, &rlp)) {
        perror("failed to get the core-file cap limit");
    } else {
        printf("core-file cap limit: %lu\n", rlp.rlim_max);
    }
}

void set_core_file_size_limit(char *arg) {
    long long new_rlim = strtoll(arg, NULL, 10);

    if (0 == new_rlim) {
        perror("invalid argument for the -C option");
        return;
    }

    struct rlimit rlp;
    if (-1 == getrlimit(RLIMIT_CORE, &rlp)) {
        perror("failed to get the core-file cap limit");
        return;
    }
    rlp.rlim_cur = new_rlim;
    if (-1 == setrlimit(RLIMIT_CORE, &rlp)) {
        perror("failed to set the core-file cap limit");
    } else {
        printf("the core-file cap limit has been set successfully\n");
    }
}

void display_current_directory() {
    char *pathname = getenv("PWD");

    if (NULL == pathname) {
        perror("failed to get the current directory");
    } else {
        printf("current directory: %s\n", pathname);
    }
}

void display_environment_variables() {
    char **ptr = environ;
    for (; *ptr != NULL; ptr++) {
        printf("%s\n", *ptr);
    }
}

void set_environment_variable(char *arg) {
    if (-1 == putenv(arg)) {
        printf("failed to set the environmental variable");
    }
}

int main(int argc, char *argv[]) {
    int c;

    while ((c = getopt(argc, argv, "ispuU:cC:dvV:")) != EOF) {
        switch (c) {
            case 'i':
                display_user_info();
                break;
            case 's':
                set_process_group();
                break;
            case 'p':
                display_process_info();
                break;
            case 'u':
                display_file_size_limit();
                break;
            case 'U':
                set_file_size_limit(optarg);
                break;
            case 'c':
                display_core_file_size_limit();
                break;
            case 'C':
                set_core_file_size_limit(optarg);
                break;
            case 'd':
                display_current_directory();
                break;
            case 'v':
                display_environment_variables();
                break;
            case 'V':
                set_environment_variable(optarg);
                break;
            case ':':
                printf("missing argument for option: %c\n", optopt);
                break;
            case '?':
            default:
                printf("invalid option: %c\n", optopt);
                print_usage();
                return 1;
        }
    }

    return 0;
}
