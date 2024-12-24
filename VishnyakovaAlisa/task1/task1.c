#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <pwd.h>
#include <grp.h>
#include <ulimit.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

void print_ids()
{
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();
    gid_t real_gid = getgid();
    gid_t effective_gid = getegid();
    printf("Real UID: %d, Effective UID: %d\n", real_uid, effective_uid);
    printf("Real GID: %d, Effective GID: %d\n", real_gid, effective_gid);
}

void print_process_ids()
{
    pid_t pid = getpid();
    pid_t ppid = getppid();
    pid_t pgid = getpgid(pid);
    printf("PID: %d, PPID: %d, PGID: %d\n", pid, ppid, pgid);
}

void print_ulimit()
{
    long limit = ulimit(UL_GETFSIZE);
    if (limit == -1)
    {
        perror("ulimit");
    }
    else
    {
        printf("Current ulimit: %ld\n", limit);
    }
}

void change_ulimit(long new_limit)
{
    if (ulimit(UL_SETFSIZE, new_limit) == -1)
    {
        perror("Error setting new ulimit");
    }
}

void print_core_size()
{
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == 0)
    {
        printf("Core file size: %lu bytes\n", rl.rlim_cur);
    }
    else
    {
        perror("getrlimit");
    }
}

void change_core_size(unsigned long new_size)
{
    struct rlimit rl;


    if (getrlimit(RLIMIT_CORE, &rl) == 0)
    {
        rl.rlim_cur = new_size;
        if (setrlimit(RLIMIT_CORE, &rl) != 0)
        {
            perror("Error setting core file size");
        }
    }
    else
    {
        perror("getrlimit");
    }
}

void print_working_directory()
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("Current working directory: %s\n", cwd);
    }
    else
    {
        perror("getcwd");
    }
}

void print_env_variables()
{
    extern char** environ;
    for (char** env = environ; *env != 0; env++)
    {
        char* thisEnv = *env;
        printf("%s\n", thisEnv);
    }
}

void set_env_variable(const char* name, const char* value)
{
    setenv(name, value, 1);
}

void change_group_leader(pid_t pid)
{
    if (setpgid(0, 0) == -1)
    {
        perror("Error changing process group leader");
    }
}

int main(int argc, char* argv[])
{
    int opt;
    char* endptr;

    while ((opt = getopt(argc, argv, "ispu:U:c:C:dvV:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            print_ids();
            break;
        case 's':
            change_group_leader(getpid());
            break;
        case 'p':
            print_process_ids();
            break;
        case 'u':
            print_ulimit();
            break;
        case 'U':
            errno = 0;
            long new_ulimit = strtol(optarg, &endptr, 10);
            if (errno != 0 || *endptr != '\0')
            {
                fprintf(stderr, "Invalid argument for -U: %s\n", optarg);
            }
            else
            {
                change_ulimit(new_ulimit);
            }
            break;
        case 'c':
            print_core_size();
            break;
        case 'C':
            errno = 0;
            unsigned long new_core_size = strtoul(optarg, &endptr, 10);
            if (errno != 0 || *endptr != '\0')
            {
                fprintf(stderr, "Invalid argument for -C: %s\n", optarg);
            }
            else
            {
                change_core_size(new_core_size);
            }
            break;
        case 'd':
            print_working_directory();
            break;
        case 'v':
            print_env_variables();
            break;
        case 'V':
            set_env_variable(optarg, argv[optind]);
            break;
        default:
            fprintf(stderr, "Usage: %s [-i] [-s] [-p] [-u] [-U new_ulimit] [-c] [-C size] [-d] [-v] [-V name=value]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    exit(0);
}