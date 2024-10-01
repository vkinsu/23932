#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

int main() {
	uid_t real_uid = getuid();
	uid_t effective_uid = geteuid();
	printf("Real UID: %d, Effective UID: %d\n", real_uid, effective_uid);

	FILE *file = fopen("myfile.txt", "r");
	if (file == NULL) {
		perror("Error opening file.");
	} else {
		printf("File opened successfully.\n");
        	fclose(file);
    	}

    	if (setuid(real_uid) == -1) {
        	perror("Error setting user id.");
        	exit(EXIT_FAILURE);
    	}

    	printf("ID have been brought to real UID: %d\n", getuid());

    	file = fopen("myfile.txt", "r");
    	if (file == NULL) {
        	perror("Error reopening file.");
    	} else {
        	printf("File opened successfully after UID change.\n");
        	fclose(file);
    	}

    	return 0;
}
