#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

#define SYSTEM_CALL "cat textfile"

main()
{
  pid_t _pid = fork();

  if (_pid == 0)
  {
    printf("[%ld] Child process, calling cat...\n", getpid());
    system(SYSTEM_CALL);
  }
  else
  {
    printf("[%ld] Parent process, waiting for [%ld]...\n", getpid(), _pid);
    waitpid(_pid, 0, 0);
    printf("[%ld] Parent process, <Hello world>\n", getpid());
  }

  return 0;
}
