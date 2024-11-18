#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <setjmp.h>

#define MAX_LINES 500

jmp_buf env;

void alarm_handler(int sig) {
	longjmp(env, 1);
}

void print_entire_file(char *file_map, off_t file_size) {
	fwrite(file_map, 1, file_size, stdout);
}

int main(int argc, char *argv[]) {
	long offsets[MAX_LINES];
	int fd;
	int current_line = 1, line_len = 0, input_line;
	char *file_map;
	struct stat sb;

	if (argc < 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	if ((fd = open(argv[1], O_RDONLY)) == -1) {
		perror("Error opening file");
		return 1;
	}

	if (fstat(fd, &sb) == -1) {
		perror("Error getting file size");
		close(fd);
		return 1;
	}

	file_map = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (file_map == MAP_FAILED) {
		perror("Error mapping file");
		close(fd);
		return 1;
	}

	offsets[1] = 0L;

	for (off_t i = 0; i < sb.st_size; i++) {
		if (file_map[i] == '\n') {
			current_line++;
			offsets[current_line] = i + 1;
		}
	}

	signal(SIGALRM, alarm_handler);

	while (1) {
		if (setjmp(env) == 0) {
			printf("Enter line number (0 to exit): ");
			alarm(5);
		if (scanf("%d", &input_line) == 1) {
			alarm(0);
		if (input_line == 0) {
			break;
		}

		if (input_line >= current_line || input_line < 1) {
			printf("Invalid line number\n");
			continue;
		}

		off_t start = offsets[input_line];
		off_t end = (input_line == current_line - 1) ? sb.st_size : offsets[input_line + 1];
		fwrite(&file_map[start], 1, end - start, stdout);
		}
		} else {
			printf("\nTime's up! Displaying entire file:\n");
			print_entire_file(file_map, sb.st_size);
			break;
		}
	}

	munmap(file_map, sb.st_size);
	close(fd);
	return 0;
}
