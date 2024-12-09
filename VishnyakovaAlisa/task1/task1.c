#include <stdio.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <errno.h>
#include <limits.h>
#include <ulimit.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char* argv[]) {
    int opt;
    long new_ulimit, new_core_size;

    while ((opt = getopt(argc, argv, "ispucC:U:dV:v")) != -1)
    {
        switch (opt)
        {
        case 'i':
            printf("Real UID: %d, Effective UID: %dn", getuid(), geteuid());
            printf("Real GID: %d, Effective GID: %dn", getgid(), getegid());
            break;

        case 's':
            if (setpgid(0, 0) == -1) 
            {
                perror("setpgid");
            }
            else 
            {
                printf("Process became the leader.n");
            }
            break;

        case 'p':
            printf("PID: %d, PPID: %d, PGID: %dn", getpid(), getppid(), getpgid(0));
            break;

        case 'u': {
            long ulimit_value = ulimit(UL_GETFSIZE);
            if (ulimit_value == -1) 
            {
                perror("ulimit");
            }
            else
            {
                printf("Ulimit: %ldn", ulimit_value);
            }
            break;
        }

        case 'c':
        {
            struct rlimit r;
            if (getrlimit(RLIMIT_CORE, &r) == 0) 
            {
                printf("Core file size: %ld bytesn", r.rlim_cur);
            }
            else 
            {
                perror("getrlimit");
            }
            break;
        }

        case 'C':
            new_core_size = atol(optarg);
            {
                struct rlimit r;
                r.rlim_cur = new_core_size;
                r.rlim_max = new_core_size;
                if (setrlimit(RLIMIT_CORE, &r) == -1)
                {
                    perror("setrlimit");
                }
                else
                {
                    printf("Core file size changed to: %ld bytesn", new_core_size);
                }
            }
            break;

        case 'U':
            new_ulimit = atol(optarg);
            if (ulimit(UL_SETFSIZE, new_ulimit) == -1) 
            {
                perror("ulimit");
            }
            else 
            {
                printf("Ulimit set to: %ldn", new_ulimit);
            }
            break;

        case 'd': 
        {
            char c[PATH_MAX];
            if (getcwd(c, sizeof(c)) != NULL)
            {
                printf("Current working directory: %sn", c);
            }
            else 
            {
                perror("getcwd");
            }
            break;
        }

        case 'v':
        {
            extern char** environ;
            char** env = environ;
            while (*env)
            {
                printf("%sn", *env);
                env++;
            }
            break;
        }

        case 'V':
            if (optarg)
            {
                char* equal_sign = strchr(optarg, '=');
                if (equal_sign == NULL)
                {
                    fprintf(stderr, "Invalid format");
                }
                else 
                {
                    *equal_sign = '0';
                    char* name = optarg;
                    char* value = equal_sign + 1;

                    if (setenv(name, value, 1) == -1)
                    {
                        perror("setenv");
                    }
                    else {
                        printf("Environment variable %s set to %sn", name, value);
                    }
                }
            }
            else 
            {
                fprintf(stderr, "Option -V requires an argument.n");
            }
            break;

        default:
            fprintf(stderr, "Use: %s [-i] [-s] [-p] [-u] [-c] [-C size] [-U new_ulimit] [-d] [-v] [-V name=value]n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

