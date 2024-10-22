#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <ulimit.h>
#include <getopt.h>
#include <sys/resource.h>
#include <sys/types.h>

#define ENV_LENGTH 16

extern char **environ;

int main(int argc, char *argv[])
{
    char options[] = "ispuU:cC:dvV:";
    int command;
    long int ulim;
    struct rlimit rlim;

    char cwd[PATH_MAX];
    char *envs;

    while ((command = getopt(argc, argv, options)) != -1) {
        switch (command) {
            case 'd':               
                if (getcwd(cwd, sizeof(cwd)) != NULL) {
                    printf("CWD: %s\n", cwd);
                }
                else {
                    perror("Error on getting current working directory\n");                
                }
                printf("\n");
                break;
            case 'v':
                printf("Environment variables:\n");
                for (char **env = environ; *env; ++env) {
                    printf("%s\n", *env);
                }
                printf("\n");
                break;
            case 'i':
                printf("User identifiers:\n");
                printf("UID: %d\n", getuid());
                printf("GID: %d\n", getgid());
                printf("EUID: %d\n", geteuid());
                printf("EGID: %d\n", getegid());
                printf("\n");
                break;
            case 'p':
                printf("Process identifiers:\n");
                printf("Current process ID: %d\n", getpid());
                printf("Parent process ID: %d\n", getppid());
                printf("Process group ID: %d\n", getpgid(0));
                printf("\n");
        		break;
            case 's':
                if (setpgid(0, getpgid(0)) == -1) {
                    perror("Error on handling process change\n");
                }
                else {
                    printf("Changed process\n");
                }
                printf("\n");
                break;
            case 'u':
                if (getrlimit(RLIMIT_FSIZE, &rlim) == 0) {
                    printf("Ulimit value: %lu\n", rlim.rlim_cur);
                }
                else{
                    perror("Failed to get ulimit value\n");
                }
                printf("\n");
                break;
            case 'c':
                getrlimit(RLIMIT_CORE, &rlim);
                printf("Core file limits:\n");
                printf("Core files soft limit: %lu\n", rlim.rlim_cur);
                printf("Core files hard limit: %lu\n", rlim.rlim_max);
                printf("\n");
                break;
            case 'V': ;
                char *env;
                char *value;

                env = (char*)malloc(ENV_LENGTH * sizeof(char));

                for (int i = 0; i < ENV_LENGTH; i++) {
                    if (optarg[i] != '=') {
                        env[i] = optarg[i];
                        value = &optarg[i];
                    }
                    else break;
                }
                value += 2;

                if (env && value) {
                    if (setenv(env, value, 1) == 0 && getenv(env)) {
                        printf("Added environment variable: %s=%s\n", env, value);
                    }
                    else {
                        perror("Error on handling setenv().\n");
                    }
                }
                else {
                    perror("Invalid environment variable format.\n");
                }
                printf("\n");
                free(env);
                break;
            case 'C':
                getrlimit(RLIMIT_CORE, &rlim);
                rlim.rlim_cur = atoi(optarg);
                rlim.rlim_max = -1;
                if (setrlimit(RLIMIT_CORE, &rlim) == -1) {
                    perror("Error on handling core file size change\n");
                }
                else {
                    printf("Core file size changed to %ld\n", rlim.rlim_cur);
                }
                printf("\n");
                break;
            case 'U':
                unsigned long int ulimit_new = strtol(optarg, NULL, 10);
                if (ulimit_new == 0) {
                    perror("Wrong value format\n");
                    break;
                }
                if (getrlimit(RLIMIT_FSIZE, &rlim) != 0) {
                    perror("Failed to set ulimit value\n");
                    break;
                }
                rlim.rlim_cur = ulimit_new;
                if (setrlimit(RLIMIT_FSIZE, &rlim) == -1) {
                    perror("Failed to set ulimit value\n");
                }
                break;
            case '?':
                printf("Invalid option is %c\n", optopt);
                printf("\n");
                break;
        }
    }
    return 0;
}
