#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>

#define MAX_LINES 500
#define BUFFER_SIZE 257

int main(int argc, char *argv[]) {
	long offsets[MAX_LINES];
	int fd;
	int current_line = 1, line_len = 0, input_line;
	char c, buffer[BUFFER_SIZE];

	if (argc < 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	if ((fd = open(argv[1], O_RDONLY)) == -1) {
		perror("Error opening file");
		return 1;
	}

	offsets[1] = 0L;

	while (read(fd, &c, 1)) {
		if (c == '\n') {
			current_line++;
			offsets[current_line] = lseek(fd, 0L, SEEK_CUR);
			line_len = 0;
		} else {
			line_len++;
		}
	}

	while (1) {
		printf("Enter line number (0 to exit): ");
		scanf("%d", &input_line);

		if (input_line == 0) {
			break;
		}

		if (input_line >= current_line || input_line < 1) {
			printf("Invalid line number\n");
			continue;
		}

		lseek(fd, offsets[input_line], SEEK_SET);

		int bytes_read = 0;
		int idx = 0;
		while ((bytes_read = read(fd, &c, 1)) > 0 && c != '\n') {
			buffer[idx++] = c;
		}
		buffer[idx] = '\0';
		printf("Line %d: %s\n", input_line, buffer);
	}

	close(fd);
	return 0;
}
