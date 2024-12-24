#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void print_permissions(mode_t mode) {
    // Функция для вывода прав доступа
    printf("%c", (S_ISDIR(mode)) ? 'd' : '-');
    printf("%c", (mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (mode & S_IXUSR) ? 'x' : '-');
    printf("%c", (mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (mode & S_IXGRP) ? 'x' : '-');
    printf("%c", (mode & S_IROTH) ? 'r' : '-');
    printf("%c", (mode & S_IWOTH) ? 'w' : '-');
    printf("%c", (mode & S_IXOTH) ? 'x' : '-');
}

void print_file_info(const char* filename) {
    struct stat file_stat;
    if (stat(filename, &file_stat) == -1) {
        perror("Не удалось получить информацию о файле");
        return;
    }

    print_permissions(file_stat.st_mode);
    printf(" %3d ", file_stat.st_nlink);

    struct group* group_info = getgrgid(file_stat.st_gid);
    struct passwd* user_info = getpwuid(file_stat.st_uid);

    printf("%s ", group_info->gr_name);
    printf("%s ", user_info->pw_name);
    printf("%7ld ", file_stat.st_size);

    char* months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

    struct tm* mod_time = localtime(&file_stat.st_mtime);
    printf("%s %2d %2d:%02d ", months[mod_time->tm_mon],
        mod_time->tm_mday, mod_time->tm_hour, mod_time->tm_min);

    const char* basename = strrchr(filename, '/');
    if (basename) {
        basename++; // Пропускаем '/'
    }
    else {
        basename = filename; // Если '/' нет, используем весь путь
    }

    printf("%s\n", basename);
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            print_file_info(argv[i]);
        }
    }

    return 0;
}
