#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>
#include <getopt.h>

extern char **environ;

void make_decision(char c)
{
    struct rlimit lim;
    switch (c)
    {
    case 'i':
        printf("Real UID: %d\n", getuid());
        printf("Effective UID: %d\n", geteuid());
        printf("Real GID: %d\n", getgid());
        printf("Effective GID: %d\n", getegid());
        break;
    case 's':
        if (setpgid(getpid(), getpid()) == 0)
        {
            printf("Done\n");
        }
        else
        {
            perror("Error\n");
        }
        break;

    case 'p':
        printf("PID: %d\n", getpid());
        printf("PPID: %d\n", getppid());
        printf("PGID: %d\n", getpgid(0));
        break;
    case 'u':
        // getrlimit(RLIMIT_FSIZE, &lim)
        printf("Soft lim: %ld\n", lim.rlim_cur);
        printf("Hard lim: %ld\n", lim.rlim_max);
        break;
    case 'U':
        getrlimit(RLIMIT_FSIZE, &lim);
        long a = atoi(optarg);
        lim.rlim_cur = a;

        getrlimit(RLIMIT_FSIZE, &lim);
        printf("New lim: %ld\n", lim.rlim_cur);
        break;

    case 'c':
        getrlimit(RLIMIT_CORE, &lim);
        printf("CORE values. Soft limit: %ld. Hard limit: %ld.\n", lim.rlim_cur, lim.rlim_max);
        break;

    case 'C':
        getrlimit(RLIMIT_CORE, &lim);
        lim.rlim_cur = (rlim_t)atoi(optarg);
        setrlimit(RLIMIT_CORE, &lim);
        printf("New lim core file size: %ld.\n", lim.rlim_cur);
        break;

    case 'd':
        char *path = getenv("PWD");
        printf("Current directory: %s\n", path);
        break;

    case 'v':
        for (char **i = environ; *i != 0; ++i)
        {
            printf("%s\n", *i);
        }
        break;

    case 'V':
        putenv(optarg);
        break;

    case '?':
        printf("Invalid option is %c\n", c);
    }
}

int main(int argc, char *argv[])
{
    char options[] = "ispuU:cC:dvV:"; /* valid options */
    int c = 0;
    char input_options[1000];
    int index = 0;

    while ((c = getopt(argc, argv, options)) != EOF)
    {
        input_options[index] = c;
        index++;
    }

    for (int i = index - 1; i >= 0; --i)
    {
        make_decision(options[i]);
    }

    return 0;
}