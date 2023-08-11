#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
  if (argc > 1) {
    printf("No arguments are used\n");
    exit(1);
  }

  // create two pipes; [0]: read, [1], write
  int ping_pipe[2];
  int pong_pipe[2];
  pipe(ping_pipe);
  pipe(pong_pipe);

  // create two processes
  int pid = fork();

  if (pid == 0) {  // child
    close(ping_pipe[1]);
    close(pong_pipe[0]);

    // read ping
    char buf[1];
    read(ping_pipe[0], buf, 1);
    printf("process %d: received ping %s\n", pid, buf);
    close(ping_pipe[0]);

    // write pong
    write(pong_pipe[1], buf, 1);
    close(pong_pipe[1]);
    
    exit(0);
  }
  else {  // parent
    close(ping_pipe[0]);
    close(pong_pipe[1]);

    // write ping
    char c[1] = "w";
    write(ping_pipe[1], c, 1);
    close(ping_pipe[1]);

    // read pong
    char buf[1];
    read(pong_pipe[0], buf, 1);
    printf("process %d: received pong %s\n", pid, buf);
    close(pong_pipe[0]);

    exit(0);
  }
}
