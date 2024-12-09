#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Copyright (C) 1992,94,1996-2000,2002,2004 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef        _SYS_RESOURCE_H
#define        _SYS_RESOURCE_H        1

/* Get the system-dependent definitions of structures and bit values.  */
#include <bits/resource.h>

#ifndef __id_t_defined
typedef __id_t id_t;
# define __id_t_defined
#endif

__BEGIN_DECLS

/* The X/Open standard defines that all the functions below must use
   `int' as the type for the first argument.  When we are compiling with
   GNU extensions we change this slightly to provide better error
   checking.  */
#if defined __USE_GNU && !defined __cplusplus
typedef enum __rlimit_resource __rlimit_resource_t;
typedef enum __rusage_who __rusage_who_t;
typedef enum __priority_which __priority_which_t;
#else
typedef int __rlimit_resource_t;
typedef int __rusage_who_t;
typedef int __priority_which_t;
#endif

/* Put the soft and hard limits for RESOURCE in *RLIMITS.
   Returns 0 if successful, -1 if not (and sets errno).  */
#ifndef __USE_FILE_OFFSET64
extern int getrlimit (__rlimit_resource_t __resource,
                      struct rlimit *__rlimits) __THROW;
#else
# ifdef __REDIRECT_NTH
extern int __REDIRECT_NTH (getrlimit, (__rlimit_resource_t __resource,
                                       struct rlimit *__rlimits), getrlimit64);
# else
#  define getrlimit getrlimit64
# endif
#endif
#ifdef __USE_LARGEFILE64
extern int getrlimit64 (__rlimit_resource_t __resource,
                        struct rlimit64 *__rlimits) __THROW;
#endif

/* Set the soft and hard limits for RESOURCE to *RLIMITS.
   Only the super-user can increase hard limits.
   Return 0 if successful, -1 if not (and sets errno).  */
#ifndef __USE_FILE_OFFSET64
extern int setrlimit (__rlimit_resource_t __resource,
                      __const struct rlimit *__rlimits) __THROW;
#else
# ifdef __REDIRECT_NTH
extern int __REDIRECT_NTH (setrlimit, (__rlimit_resource_t __resource,
                                       __const struct rlimit *__rlimits),
                           setrlimit64);
# else
#  define setrlimit setrlimit64
# endif
#endif
#ifdef __USE_LARGEFILE64
extern int setrlimit64 (__rlimit_resource_t __resource,
                        __const struct rlimit64 *__rlimits) __THROW;
#endif

/* Return resource usage information on process indicated by WHO
   and put it in *USAGE.  Returns 0 for success, -1 for failure.  */
extern int getrusage (__rusage_who_t __who, struct rusage *__usage) __THROW;

/* Return the highest priority of any process specified by WHICH and WHO
   (see above); if WHO is zero, the current process, process group, or user
   (as specified by WHO) is used.  A lower priority number means higher
   priority.  Priorities range from PRIO_MIN to PRIO_MAX (above).  */
extern int getpriority (__priority_which_t __which, id_t __who) __THROW;

/* Set the priority of all processes specified by WHICH and WHO (see above)
   to PRIO.  Returns 0 on success, -1 on errors.  */
extern int setpriority (__priority_which_t __which, id_t __who, int __prio)
     __THROW;

__END_DECLS

#endif        /* sys/resource.h  */


void print_user_group_ids() {
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();
    gid_t real_gid = getgid();
    gid_t effective_gid = getegid();

    printf("Real UID: %d, Effective UID: %d\n", real_uid, effective_uid);
    printf("Real GID: %d, Effective GID: %d\n", real_gid, effective_gid);
}

void print_process_ids() {
    pid_t pid = getpid();
    pid_t ppid = getppid();
    pid_t pgid = getpgid(pid);

    printf("Process ID: %d, Parent Process ID: %d, Process Group ID: %d\n", pid, ppid, pgid);
}

void print_ulimit() {
    struct rlimit rl;
    if (getrlimit(RLIMIT_FSIZE, &rl) == 0) {
        printf("Ulimit (file size): %ld\n", rl.rlim_cur);
    } else {
        perror("getrlimit");
    }
}

void set_ulimit(const char *new_ulimit) {
    long new_limit = strtol(new_ulimit, NULL, 10);
    struct rlimit rl;
    rl.rlim_cur = new_limit;
    rl.rlim_max = new_limit;

    if (setrlimit(RLIMIT_FSIZE, &rl) == 0) {
        printf("Ulimit set to: %ld\n", new_limit);
    } else {
        perror("setrlimit");
    }
}

void print_core_file_size() {
    struct rlimit rl;
    if (getrlimit(RLIMIT_CORE, &rl) == 0) {
        printf("Core file size: %ld\n", rl.rlim_cur);
    } else {
        perror("getrlimit");
    }
}

void set_core_file_size(const char *size) {
    long new_size = strtol(size, NULL, 10);
    struct rlimit rl;
    rl.rlim_cur = new_size;
    rl.rlim_max = new_size;

    if (setrlimit(RLIMIT_CORE, &rl) == 0) {
        printf("Core file size set to: %ld\n", new_size);
    } else {
        perror("setrlimit");
    }
}

void print_current_directory() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd");
    }
}

void print_environment_variables() {
    extern char **environ;
    for (char **env = environ; *env != 0; env++) {
        char *thisEnv = *env;
        printf("%s\n", thisEnv);
    }
}

void set_environment_variable(const char *name_value) {
    char *name = strdup(name_value);
    char *value = strchr(name, '=');
    if (value != NULL) {
        *value = '\0';
        value++;
        if (setenv(name, value, 1) == 0) {
            printf("Environment variable set: %s=%s\n", name, value);
        } else {
            perror("setenv");
        }
    } else {
        fprintf(stderr, "Invalid format for environment variable: %s\n", name_value);
    }
    free(name);
}

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "ispucC:dU:vV:")) != -1) {
        switch (opt) {
            case 'i':
                print_user_group_ids();
                break;
            case 's':
                if (setpgid(0, 0) == 0) {
                    printf("Process became the group leader.\n");
                } else {
                    perror("setpgid");
                }
                break;
            case 'p':
                print_process_ids();
                break;
            case 'u':
                print_ulimit();
                break;
            case 'U':
                set_ulimit(optarg);
                break;
            case 'c':
                print_core_file_size();
                break;
            case 'C':
                set_core_file_size(optarg);
                break;
            case 'd':
                print_current_directory();
                break;
            case 'v':
                print_environment_variables();
                break;
            case 'V':
                set_environment_variable(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-i] [-s] [-p] [-u] [-U new_ulimit] [-c] [-C size] [-d] [-v] [-V name=value]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    return 0;
}

