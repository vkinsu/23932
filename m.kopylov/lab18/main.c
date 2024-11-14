#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

static char* months[] = {"Jan","Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static char buffer[256];
static char* ptr_buffer;

#define TAB_SIZE 6
#define TAB(s) SUBTAB(s)
#define SUBTAB(s) #s

#define USER 0
#define GROUP 1
#define ALL 2

void get_name(char* path)
{
    int n = strlen(path);
    assert(n < 128);
    ptr_buffer = &buffer[128];
    int buff_index = 0;
    for (int i = n-1; i > 0 || path[i] != '/';i--)
    {
        ptr_buffer[buff_index++] = path[i];
    }

    int i;
    for (i = 0; i < n; i++)
    {
        buffer[i] = ptr_buffer[n-i-1];
    }
    buffer[i] = '\0';
}

void p_type(unsigned int mode)
{
    if (mode & S_IFREG)       printf("-"); //FILE
    else if (mode & S_IFDIR)  printf("d"); // Dir
    else                      printf("?");
}

void p_access(unsigned int mode, unsigned int target)
{
    switch (target)
    {
    case USER:
        printf("%c%c%c",    mode & S_IRUSR ? 'r' : '-',
                            mode & S_IWUSR ? 'w' : '-',
                            mode & S_IXUSR ? 'x' : '-');
        
        break;
    case GROUP:
        printf("%c%c%c",    mode & S_IRGRP ? 'r' : '-',
                            mode & S_IWGRP ? 'w' : '-',
                            mode & S_IXGRP ? 'x' : '-');
        break;
    case ALL:
        printf("%c%c%c",    mode & S_IROTH ? 'r' : '-',
                            mode & S_IWOTH ? 'w' : '-',
                            mode & S_IXOTH ? 'x' : '-');
        break;
    default:
        break;
    }
}

void p_links(unsigned int nlink)
{
    printf("%2u", nlink);
}

void p_names(unsigned int guid, unsigned int target)
{
    
    switch (target)
    {
    case USER:
        {
            struct passwd* ptr_passwd = getpwuid(guid);
            if (ptr_passwd == NULL) 
            {
                exit(-1);
            }
            printf("%"TAB(TAB_SIZE)"s", ptr_passwd->pw_name);
        }
        break;
    case GROUP:
        {
            struct group* ptr_group = getgrgid(guid);
            if (ptr_group == NULL) 
            {
                exit(-1);
            }
            
            printf("%"TAB(TAB_SIZE)"s", ptr_group->gr_name);
        }
        break;
    
    default:
        break;
    }
}

void p_size(long size, unsigned int mode)
{
    char w_space = ' ';
    if (mode & S_IFREG)  printf("%"TAB(TAB_SIZE)"ld", size); //FILE
    else printf("%"TAB(TAB_SIZE)"c", w_space);
}

void p_mtime(time_t time)
{
    struct tm* ptr_tm = localtime(&time);
    printf("%4s%3d%5d", months[ptr_tm->tm_mon], ptr_tm->tm_mday, ptr_tm->tm_year+1900);
}

void p_filename()
{
    printf(" %s", buffer);
}

void print_stat(struct stat* stat)
{
    p_type(stat->st_mode);
    p_access(stat->st_mode, USER);
    p_access(stat->st_mode, GROUP);
    p_access(stat->st_mode, ALL);
    p_links(stat->st_nlink);
    p_names(stat->st_uid, USER);
    p_names(stat->st_uid, GROUP);
    p_size(stat->st_size, stat->st_mode);
    p_mtime(stat->st_mtime);
    p_filename();

    printf("\n");
}

int main(int argc, char** argv)
{
    assert(argc > 1);
    int res;
    struct  stat stat_buffer;
    for (int i = 1; i < argc; i++)
    {
        res = stat(argv[i], &stat_buffer);
        if (res == -1)
        {
            perror("Can't read file info\n");
            exit(-1);
        }
        get_name(argv[i]);
        print_stat(&stat_buffer);
    }

    return 0;
}
