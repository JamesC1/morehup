#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[]) {
  int fd = 0;
  int sig = SIGHUP;

  int rest_argc = argc-1;
  char **rest_argv = argv+1;

  while (rest_argc) {
    int progress = 0;
    if (!strcmp(rest_argv[0], "-f")) {
      --rest_argc;
      ++rest_argv;
      if (rest_argc) {
        fd=atoi(rest_argv[0]);
        --rest_argc;
        ++rest_argv;
        ++progress;
      }else{
        break;
      }
    }
    else if (!strcmp(rest_argv[0], "-s")) {
      --rest_argc;
      ++rest_argv;
      if (rest_argc) {
        sig=atoi(rest_argv[0]);
        --rest_argc;
        ++rest_argv;
        ++progress;
      }else{
        break;
      }
    }
    if (!progress)
      break;
  }

  if (rest_argc < 1 || '-' == rest_argv[0][0] ) {
    fprintf(stderr, "\
Usage: [options] prog args\n\
   [-f fd]      (default 0)\n\
   [-s signal]  (default SIGHUP)\n");
    exit(1);
  }

  int init_flags=fcntl(fd, F_GETFL, 0);
  int async = O_ASYNC;

  if (fcntl(fd, F_SETFL, init_flags | async)) {
    perror("fcntl...F_SET_FL....O_ASYNC");
    exit(1);
  }

  int next_flags=fcntl(fd, F_GETFL, 0);

  printf("Stdin flags: %o -> %o wrt %o\n", init_flags, next_flags, async);

  int init_owner=fcntl(fd, F_GETOWN);

  if (fcntl(fd, F_SETOWN, getpid())) {
    perror("fcntl(0, F_SETOWN, ..)");
    exit(1);
  }

  int next_owner=fcntl(fd, F_GETOWN);

  printf("Signal owner: %i -> %i wrt %i\n", init_owner, next_owner, getpid());

  int init_signal=fcntl(fd, F_GETSIG);

  if (fcntl(fd, F_SETSIG, sig)) {
    perror("fcntl(0, F_SETSIG, ..)");
    exit(1);
  }

  int next_signal=fcntl(fd, F_GETSIG);

  printf("Signal number: %d -> %d wrt %d\n", init_signal, next_signal, sig);


  if (execve(rest_argv[0], rest_argv, envp)) {
    perror("execve");
    exit(1);
  }

  return 0;
}
