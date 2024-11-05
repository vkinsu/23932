#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include <string.h>
#include <sys/resource.h>

void print_core_file_size() {
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == 0) {
        printf("Core file size: %ld\n", rl.rlim_cur);
    } else {
        perror("getrlimit");
    }
}

void print_ids() {
    uid_t ruid, euid;
    gid_t rgid, egid;
    ruid = getuid();
    euid = geteuid();
    rgid = getgid();
    egid = getegid();

    printf("Real UID: %d, Effective UID: %d\n", ruid, euid);
    printf("Real GID: %d, Effective GID: %d\n", rgid, egid);
}

void print_process_ids() {
    printf("Process ID: %d\n", getpid());
    printf("Parent Process ID: %d\n", getppid());
    printf("Process Group ID: %d\n", getpgrp());
}

void print_ulimit() {
    struct rlimit rl;
    if (getrlimit(RLIMIT_FSIZE, &rl) == 0) {
        printf("Current ulimit (max file size): soft: %lu, hard: %lu\n", (unsigned long)rl.rlim_cur, (unsigned long)rl.rlim_max);
    } else {
        perror("getrlimit() error");
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
void print_cwd() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd() error");
    }
}

void print_env_vars() {
    extern char **environ;
    for (char **env = environ; *env; ++env) {
        printf("%s\n", *env);
    }
}
void set_env_var(const char *name_value) {
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

int main(int argc, char *argv[]) {
    int opt;
    unsigned long new_ulimit;

    while ((opt = getopt(argc, argv, "ispucC:dU:vV:")) != -1) {
        switch (opt) {
            case 'i':
                print_ids();
                break;
            case 's':
                if (setpgid(0, 0) == -1) {
                    perror("Failed to set process group");
                } else {
                    printf("Process is now a group leader.\n");
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
            case 'd':
                print_cwd();
                break;
            case 'v':
                print_env_vars();
                break;
	    case 'C':
		set_core_file_size(optarg);
		break;
            case 'V':
		set_env_var(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-i] [-s] [-p] [-u] [-U new_ulimit] [-c] [-d] [-v] [-V name=value]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    return 0;
}

