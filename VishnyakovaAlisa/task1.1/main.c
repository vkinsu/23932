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

void set_ulimit(const char* new_ulimit)
{
            long new_limit = strtol(new_ulimit, NULL, 10);
                struct rlimit rl;
                    rl.rlim_cur = new_limit;
                        rl.rlim_max = new_limit;

                            if (setrlimit(RLIMIT_FSIZE, &rl) == 0) {
                                            printf("Ulimit set to: %ld\n", new_limit);
                                                }
                                else {
                                                perror("setrlimit\n");
                                                    }
}


void set_file_size(const char* size)
{
            long new_size = strtol(size, NULL, 10);
                struct rlimit rl;
                    rl.rlim_cur = new_size;
                        rl.rlim_max = new_size;

                            if (setrlimit(RLIMIT_CORE, &rl) == 0)
                                        {
                                                        printf("Core file size set to: %ld\n", new_size);
                                                            }
                                else
                                            {
                                                            perror("setrlimit\n");
                                                                }
}

void print_variab()
{
            extern char** environ;
                for (char** env = environ; *env != 0; env++) {
                                char* thisEnv = *env;
                                        printf("%s\n", thisEnv);
                                            }
}

void set_varieb(const char* name_value) {
            char* n = strdup(name_value);
                char* v = strchr(n, '=');
                    if (v != NULL)
                                {
                                                *v = '\0';
                                                        v++;
                                                                if (setenv(n, v, 1) == 0)
                                                                                {
                                                                                                    printf("Environment variable set: %s=%s\n", n, v);
                                                                                                            }
                                                                        else
                                                                                        {
                                                                                                            perror("setenv\n");
                                                                                                                    }
                                                                            }
                        else
                                    {
                                                    fprintf(stderr, "Invalid format for environment variable: %s\n", name_value);
                                                        }
                            free(n);
}
int main(int argc, char* argv[]) {
            int opt;
                long new_ulimit, new_core_size;
                    struct rlimit rl;
                        char cwd1[1024];
                            if (argc == 1)
                                        {
                                                        printf("Using:\n%s [-i] [-s] [-p] [-u] [-c] [-C size] [-U new_ulimit] [-d] [-v] [-V name=value]\n", argv[0]);
                                                                fprintf(stderr, "Using:\n%s [-i] [-s] [-p] [-u] [-c] [-C size] [-U new_ulimit] [-d] [-v] [-V name=value]\n", argv[0]);
                                                                        exit(EXIT_FAILURE);
                                                                            }
                                while ((opt = getopt(argc, argv, "ispucC:U:dV:v")) != -1)
                                            {
                                                            switch (opt)
                                                                            {
                                                                                            case 'i':
                                                                                                                printf("Real UID: %d, Effective UID: %d\n", getuid(), geteuid());
                                                                                                                            printf("Real GID: %d, Effective GID: %d\n", getgid(), getegid());
                                                                                                                                        break;

                                                                                                                                             case 's':
                                                                                                                                                 if (setpgid(0, 0) == -1)

                        {

                                                perror("setpgid");

                                                            }

                else

                                    {

                                                            printf("Process became the leader\n");

                                                                        }

                            break;


                                    case 'p':

                                        printf("PID: %d, PPID: %d, PGID: %d\n", getpid(), getppid(), getpgid(0));

                                                    break;

                                                            case 'u':

                                                                if (getrlimit(RLIMIT_FSIZE, &rl) == 0)

                                                                                    {

                                                                                                            printf("Ulimit (file size): %ld\n", rl.rlim_cur);

                                                                                                                        }

                                                                            else

                                                                                                {

                                                                                                                        perror("getrlimit");

                                                                                                                                    }

                                                                                        break;

                                                                                                case 'U':

                                                                                                    set_ulimit(optarg);

                                                                                                                break;

                                                                                                                        case 'c':

                                                                                                                            if (getrlimit(RLIMIT_CORE, &rl) == 0)

                                                                                                                                                {


                        printf("Core file size: %ld\n", rl.rlim_cur);


                                    }

                                                                                                                                        else {


                perror("getrlimit");


                            };

                                                                                                                                                 break;


            case 'C':


                set_file_size(optarg);


                            break;


                                    case 'd':





                                        if (getcwd(cwd1, sizeof(cwd1)) != NULL)


                                                            {


                                                                                    printf("Current working directory: %s\n", cwd1);


                                                                                                }


                                                    else


                                                                        {


                                                                                                perror("getcwd");


                                                                                                            }


                                                                break;


                                                                        case 'v':


                                                                            print_variab();


                                                                                        break;


                                                                                                case 'V':


                                                                                                    set_varieb(optarg);


                                                                                                                break;



                                                                                                                        default:


                                                                                                                            printf( "Using:\n%s [-i] [-s] [-p] [-u] [-c] [-C size] [-U new_ulimit] [-d] [-v] [-V name=value]\n", argv[0]);


                                                                                                                                        fprintf(stderr, "Using:\n%s [-i] [-s] [-p] [-u] [-c] [-C size] [-U new_ulimit] [-d] [-v] [-V name=value]\n", argv[0]);


                                                                                                                                                 exit(EXIT_FAILURE);



            }
                                                                }

                                    return 0;
}