#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

#define MAX_PATH_LENGTH 1024

void print_permissions(mode_t mode) {
	if (S_ISDIR(mode)) {
		printf("d");
	} else if (S_ISREG(mode)) {
		printf("-");
	} else {
		printf("?");
	}

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

void print_file_info(const char *file_path) {
	struct stat file_stat;
	if (stat(file_path, &file_stat) == -1) {
		perror("stat");
		return;
	}

	print_permissions(file_stat.st_mode);
	printf(" %2ld", file_stat.st_nlink);

	struct passwd *pw = getpwuid(file_stat.st_uid);
	struct group *gr = getgrgid(file_stat.st_gid);

	if (pw != NULL && gr != NULL) {
		printf(" %-8s %-8s", pw->pw_name, gr->gr_name);
	} else {
		printf(" %-8s %-8s", "unknown", "unknown");
	}

	if (S_ISREG(file_stat.st_mode)) {
		printf(" %8ld", file_stat.st_size);
	} else if ((S_ISDIR(file_stat.st_mode))) {
		printf(" %8ld", 4096);
	} else {
		printf("         ");
	}

	char time_str[64];
	struct tm *tm_info = localtime(&file_stat.st_mtime);
	strftime(time_str, sizeof(time_str), "%b %d %H:%M", tm_info);
	printf(" %-12s", time_str);

	const char *file_name = strrchr(file_path, '/');
	if (file_name) {
		file_name++;
	} else {
		file_name = file_path;
	}
	printf(" %s\n", file_name);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <file1> <file2> ...\n", argv[0]);
		return 1;
	}

	for (int i = 1; i < argc; i++) {
		print_file_info(argv[i]);
	}
	
	return 0;
}

