#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ulimit.h>
#include <string.h>


int main(int argc, char *argv[], char *envp[])
{
    int opt;
    long new_val;
    char pth[1000];
    char env_var[100];
    char env_val[100];
    int i = 0;

    

    env_var[0] = '\0';
    env_val[0] = '\0';

    while((opt = getopt(argc, argv, ":ispuU:cC:dvV:")) != -1)
    {
        switch(opt)
        {
            case 'i':
		printf("RUID = %d\n", getuid());
		printf("EUID = %d\n", geteuid());
		break;
            case 's':
		if (setpgid(110, 110) == -1)
		{
			perror("Unable to set process as leader\n");
			exit(13);
		}
		break;
            case 'p':
                printf("PID = %d\n", getpid());
		printf("Parent PID = %d\n", getppid());
		printf("PGID = %d\n", getpgid(0));
                break;
            case 'u':
                printf("ulimit = %d\n", ulimit(UL_GETFSIZE));
                break;
	    case 'U':
		new_val = atol(optarg);
		int cond = ((int)new_val != -1 && (int)new_val <= 0);
		if (cond != 0)
		{
			perror("Incorrect value for U\n");
			exit(13);
		}
		if (ulimit(UL_SETFSIZE, new_val) == -1)
		{
			perror("Unable to set ulimit\n");
			exit(13);
		}
		printf("Ulimit is now set to %d\n", new_val);
		break;
	    case 'c':
		system("ulimit -c");
		break;
	    case 'C':
		new_val = atol(optarg);
		if (new_val % 2 != 0)
		{
			perror("Core limit must be evet to 2\n");
			exit(13);
		}
		strcpy(env_val, "ulimit -c ");
		strcat(env_val, optarg);
		system(env_val);
		break;
	    case 'd':
		getcwd(pth, 1000);
		printf("Current dir: %s\n", pth);
		break;
	    case 'v':
		while(*envp)
        		printf("%s\n",*envp++);
		break;
	    case 'V':
		// String parsing was optional (((
		while(optarg[i] != '=')
		{
			if (optarg[i] == '\0')
			{
				perror("Environment variable value not stated\n");
				exit(13);
			}
			int indx = strlen(env_var);
			env_var[indx] = optarg[i];
			env_var[indx+1] = '\0';
			i++;
		}
		i++;
		while(optarg[i] != '\0')
		{
			int indx = strlen(env_val);
			env_val[indx] = optarg[i];
			env_val[indx+1] = '\0';
			i++;
		}
		i = 0;
		if (putenv(optarg) != 0)
		{
			perror("Unable to set environment variable\n");
			exit(13);
		}
		printf("Variable %s is set to %s\n", env_var, getenv(env_var));
		i = 0;
		env_var[0] = '\0';
		env_val[0] = '\0';
		break;
            case ':':
                perror("Error: option without a value\n");
		exit(13);
                break;
            case '?':
                printf("Unknown option: %c\n", optopt);
                break;
        }
    }


    return 0;
}
