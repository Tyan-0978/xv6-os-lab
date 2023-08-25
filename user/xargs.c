#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
  if (argc == 1) {
    printf("Usage: xargs [command] ...\n");
    exit(1);
  }

  // copy command and arguments
  char* exec_args[argc];
  for (int i = 1; i < argc; i++) {
    exec_args[i-1] = argv[i];
  }

  // read from standard input
  char buf[512];
  char* buf_ptr = buf;
  while (read(0, buf_ptr, 1) != 0) {
    if (*buf_ptr == '\n') {
      // execute command
      *buf_ptr = 0;
      exec_args[argc-1] = buf;
      
      int pid = fork();
      if (pid == 0) {
        // child
        exec(exec_args[0], exec_args);
        printf("exec failed\n");
      }
      else {
        // parent
        wait(0);
      }

      // reset buffer pointer
      buf_ptr = buf;
    }
    else {
      buf_ptr++;
    }
  }

  // execute one more time when read returns 0
  *buf_ptr = 0;
  exec_args[argc-1] = buf;

  int pid = fork();
  if (pid == 0) {
    // child
    exec(exec_args[0], exec_args);
    printf("exec failed\n");
  }
  else {
    // parent
    wait(0);
  }

  exit(0);
}
