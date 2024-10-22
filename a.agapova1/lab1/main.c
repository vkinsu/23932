#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/param.h>
#include <pwd.h>
#include <errno.h>
#include <string.h>

void print_user_group_ids() {
	printf("Real UID: %d, Effective UID: %d\n", getuid(), geteuid());
	printf("Real GID: %d, Effective GID: %d\n", getgid(), getegid());
}

void print_process_ids() {
	printf("PID: %d, PPID: %d, PGID: %d\n", getpid(), getppid(), getpgid(0));
}

void print_ulimit() {
	struct rlimit rl;
	if (getrlimit(RLIMIT_FSIZE, &rl) == 0) {
		printf("File size limit (ulimit): %lu bytes\n", rl.rlim_cur);
	} else {
		perror("Failed to get ulimit");
	}
}

void set_ulimit(unsigned long new_ulimit_value) {
	struct rlimit rl;
	rl.rlim_cur = new_ulimit_value;
	rl.rlim_max = RLIM_INFINITY; // Allow to set unlimited maximum
	if (setrlimit(RLIMIT_FSIZE, &rl) == 0) {
		printf("New ulimit set: %lu bytes\n", new_ulimit_value);
	} else {
		perror("Failed to set new ulimit");
	}
}

void print_core_file_size() {
	struct rlimit rl;
	if (getrlimit(RLIMIT_CORE, &rl) == 0) {
		printf("Core file size limit: %lu bytes\n", rl.rlim_cur);
	} else {
		perror("Failed to get core file size limit");
	}
}

void set_core_file_size(unsigned long new_core_size) {
	struct rlimit rl;
	rl.rlim_cur = new_core_size;
	rl.rlim_max = RLIM_INFINITY;
	if (setrlimit(RLIMIT_CORE, &rl) == 0) {
		printf("New core file size limit set: %lu bytes\n", new_core_size);
	} else {
		perror("Failed to set new core file size limit");
	}
}

void print_working_directory() {
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		printf("Current working directory: %s\n", cwd);
	} else {
		perror("Failed to get current working directory");
	}
}

void print_environment_vars() {
	extern char **environ;
	for (char **env = environ; *env; ++env) {
		printf("%s\n", *env);
	}
}

void set_environment_var(char *name_value) {
	if (putenv(name_value) == 0) {
		printf("Environment variable set: %s\n", name_value);
	} else {
		perror("Failed to set environment variable");
	}
}

int main(int argc, char *argv[]) {
	int opt;
	while ((opt = getopt(argc, argv, "ispucC:dvV:U:")) != -1) {
		switch (opt) {
		case 'i':
			print_user_group_ids();
			break;
		case 's':
			if (setpgid(0, 0) == 0) {
				printf("Process became group leader\n");
			} else {
				perror("Failed to become group leader");
			}
			break;
		case 'p':
			print_process_ids();
			break;
		case 'u':
			print_ulimit();
			break;
		case 'U':
			set_ulimit(atol(optarg));
			break;
		case 'c':
			print_core_file_size();
			break;
		case 'C':
			set_core_file_size(atol(optarg));
			break;
		case 'd':
			print_working_directory();
			break;
		case 'v':
			print_environment_vars();
			break;
		case 'V':
			set_environment_var(optarg);
			break;
		default:
			fprintf(stderr, "Usage: %s [-ispucC:dvV:U:]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}

