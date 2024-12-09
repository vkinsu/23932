#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <ulimit.h>
#include <limits.h>

#define BUFF_SIZE 256
extern char *optarg;
extern int optind, opterr, optopt;
static char env_names[BUFF_SIZE] = {0};
static char env_values[BUFF_SIZE] = {0};
extern char** environ;

void set_env(char* optarg)
{
        int n = strlen(optarg);
        int i = 0;
        int index = 0;
        for (; optarg[i] != '='; i++)
        {
                if (optarg[i] != ' ') env_names[index++] = optarg[i];
        }
        env_names[index + 1] = '\0';
        index = 0;
        i++;
        for (; i < n; i++)
        {
                if (optarg[i] != ' ') env_values[index++] = optarg[i];
        }
        env_values[index+1] = '\0';
}


int main(int argc, char** argv, char** envp)
{
    
    int opt;
    long int res; 
    void* ptr;
    struct rlimit rl;
    char buff[BUFF_SIZE];
      
    while((opt = getopt(argc, argv, ":ispuU:C:cdvV:")) != -1)  
    {  
        switch(opt)  
        {  
            case 'i':  
                printf("real uid:\t%d\neffective uid:\t%d\nreal gid:\t%d\neffective gid:\t%d\n", 
                getuid(), geteuid(), getgid(), getegid());
                break;  
            case 's':  
                printf("set %d in %d group\n", getpid(), getgid());  
                if (setpgid(0, 0) == -1)
                {
                    perror("Error in setting new id\n");
                }
                else
                {
                    printf("current gid:\t%d\n", getpid());  
                }
                break;  
            case 'p':  
                printf("pid:\t%d\nppid:\t%d\npgid:\t%d\n", getpid(), getppid(), getpgrp());  
                break;  
            case 'u':  
                res = ulimit(UL_GETFSIZE);
                if (res != -1)
                {
                    printf("Current limit:\t%lu\n",ulimit(UL_GETFSIZE));  
                }
                else
                {
                    printf("Error in getting limit");
                }   
                break;  
            case 'U':
		res = atol(optarg);	
                if (res > 0) 
                {
		    ulimit(UL_SETFSIZE,res);
                    printf("The limit has changed successfuly\n");
                    printf("Current limit:\t%lu\n",ulimit(UL_GETFSIZE));  
                }
                else
                {
                    printf("Error in changing limit\n");
                }
                break;
            case 'c':
                res = getrlimit(RLIMIT_CORE, &rl);
                if (res != -1)
                {
                    printf("Soft:\t%lu\nHard:\t%lu\n", rl.rlim_cur, rl.rlim_max);
                }
                else
                {
                    printf("Error in getting limit\n");
                }
                break;
            case 'C':
		res = atol(optarg);
		getrlimit(RLIMIT_CORE,&rl);
               	if (res > 0 && res <= rl.rlim_max)  
                {
		    rl.rlim_cur = res;
		    setrlimit(RLIMIT_CORE, &rl);
                    printf("The limit has changed successfully\n");
                }
                else
                {
                    printf("Error in getting limit\n");
                }
                break;
            case 'd':
                ptr = (void*)getcwd(buff, BUFF_SIZE);
                if (ptr == NULL)
                {
                    printf("Undefined behavior. The program will shut down with code -1\n");
                    exit(-1);
                }
                else
                {
                    printf("cwd:\t%s\n", buff);
                }
                break;
            case 'v':
                for (char **env = environ; *env != 0; env++)
                {
                    char *thisEnv = *env;
                    printf("%s\n", thisEnv);
                }
                break;
            case 'V':
		set_env(optarg);
                if (putenv(optarg) == 0)
                {
                    printf("The variable has changed successfully\n");
		    printf("%s=%s\n", env_names, getenv(env_names));
                }
                else
                {
                    printf("Error in changig variable\n");
                }
                break;
            case ':':
                printf("Arguments need value\n");
                break;
            case '?':
                printf("Undefined option: %c\n", optopt);
                break;
            default:
                break;
        }  
    }  
      
	for(; optind < argc; optind++)
	{      
        	printf("Extra arguments: %s\n", argv[optind]);  
    	} 
      
    
    return 0; 
}
