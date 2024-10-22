#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024

int get_line_number() {
	char buffer[100];
	int i = 0;
	int line_number = 0;

	printf("Enter the line number (0 to exit): ");
	fgets(buffer, sizeof(buffer), stdin);

	while (buffer[i]) {
		if (isdigit(buffer[i])) {
			line_number = line_number * 10 + (buffer[i] - '0');
		}
		i++;
	}

	return line_number;
}

int main() {
	int fd;
	off_t offset = 0;
	int line_number = 0;
	ssize_t bytes_read;
	char buffer[MAX_LINE_LENGTH];
	off_t *offset_table = NULL;
	int *line_lengths = NULL;
	int lines_count = 0;
	char ch;

	fd = open("file.txt", O_RDONLY);
	if (fd == -1) {
		perror("Error opening file");
		exit(1);
	}

	while ((bytes_read = read(fd, &ch, 1)) > 0) {
		if (ch == '\n') {
			lines_count++;
			offset_table = realloc(offset_table, sizeof(off_t) * lines_count);
			line_lengths = realloc(line_lengths, sizeof(int) * lines_count);
			offset_table[lines_count - 1] = offset;
			line_lengths[lines_count - 1] = lseek(fd, 0L, SEEK_CUR) - offset;
			offset = lseek(fd, 0L, SEEK_CUR);
		}
	}

	printf("Line offset table:\n");
	for (int i = 0; i < lines_count; i++) {
		printf("Line %d: offset = %lld, length = %d\n", i + 1, (long long)offset_table[i], line_lengths[i]);
	}

	while (1) {
		line_number = get_line_number();

		if (line_number == 0) {
			break;
		}

		if (line_number < 1 || line_number > lines_count) {
			printf("Invalid line number. Try again.\n");
			continue;
		}

		lseek(fd, offset_table[line_number - 1], SEEK_SET);

		bytes_read = read(fd, buffer, line_lengths[line_number - 1]);
		if (bytes_read > 0) {
			buffer[bytes_read] = '\0';
			printf("Line %d: %s\n", line_number, buffer);
		}
	}

	free(offset_table);
	free(line_lengths);

	close(fd);

	return 0;
}
