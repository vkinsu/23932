#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <ulimit.h>
#include <sys/resource.h>
#include <string.h>

extern char *optarg;
extern char **environ;
extern int optopt;

int main(int argc, char** argv)
{
    struct rlimit rlp;
    int opt;
    long int newlimit;
    char *cwd;
    char **env;
    char* opts = "ispucdvU:C:V:";
    while ((opt = getopt(argc, argv, opts)) != -1)
    {
        switch(opt)
        {
            case 'i':
                printf("uid = %d\n", getuid());
                printf("euid = %d\n", geteuid());
                printf("gid = %d\n", getgid());
                printf("getegid = %d\n", getegid());
                break;
            case 's':
                setpgid(0,0);
                break;
            case 'p':
                printf("pid = %d\n", getpid());
                printf("ppid = %d\n", getppid());
                printf("pgid = %d\n", getpgrp());
                break;
            case 'u':
                printf("size of ulimit is %ld\n", ulimit(UL_GETFSIZE));
                break;
            case 'U':
                if (ulimit(UL_SETFSIZE, atol(optarg)) == -1) 
                {
                    perror("Error changing size of ulimit");
                    exit(EXIT_FAILURE);
                }
                printf("Changing size to %ld\n", atol(optarg));
                break;
            case 'c':
                if (getrlimit(RLIMIT_CORE, &rlp) == -1) {
                    perror("Error getrlimit");
                    exit(EXIT_FAILURE);
                }
                printf("current = %ld, max = %ld\n", rlp.rlim_cur, rlp.rlim_max);
                break;
            case 'C':
                newlimit = atol(optarg);
                getrlimit(RLIMIT_CORE, &rlp);
                rlp.rlim_cur = newlimit;
                setrlimit(RLIMIT_CORE, &rlp);
                break;
            case 'd':
                cwd = getcwd(NULL, 0);
                printf("current directory: %s\n", cwd);
                free(cwd);
                break;
            case 'v':
                env = environ;
                while (*env)
                {
                    printf("%s\n", *env);
                    env++;
                }
                break;
            case 'V':
                setenv("NEW_VAR", optarg, 1);
                break;
            default:
                fprintf(stderr, "unknown parameter: %c\n", optopt);
                exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}


