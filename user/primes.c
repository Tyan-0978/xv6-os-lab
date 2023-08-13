#include "kernel/types.h"
#include "user/user.h"

void propagate_prime(int read_fd);

int main() {
  // create pipe
  int p[2];
  pipe(p);
  
  // fork process
  int pid = fork();
  if (pid == 0) {  // child
    // close write end of pipe
    close(p[1]);

    // recursive call to propagate numbers to child processes
    propagate_prime(p[0]);
    close(p[0]);
  }
  else {  // parent
    // close read end of pipe
    close(p[0]);

    // write 2-35 into pipe
    for (int i = 2; i <= 35; ++i) {
      write(p[1], &i, sizeof(i));
    }
    close(p[1]);

    // wait for child processes to exit
    wait(0);
  }

  exit(0);
}

void propagate_prime(int pipe_read_fd) {
  // set base prime
  int base_prime;
  read(pipe_read_fd, &base_prime, sizeof(base_prime));
  printf("prime %d\n", base_prime);

  // read next number; check if the stream has ended
  int read_bytes;
  int next_num;
  read_bytes = read(pipe_read_fd, &next_num, sizeof(next_num));
  if (read_bytes == 0) {
    return;
  }

  // create pipe and fork processes
  int p[2];
  pipe(p);

  int pid = fork();
  if (pid == 0) {  // child
    // close write end of pipe
    close(p[1]);

    // recursive call to propagate numbers to child processes
    propagate_prime(p[0]);
    close(p[0]);
    exit(0);
  }
  else {  // parent
    // close read end of pipe
    close(p[0]);

    // propagate numbers to child process
    while (read_bytes) {
      // write number to pipe
      if (next_num % base_prime != 0) {
        write(p[1], &next_num, sizeof(next_num));
      }

      // read next number
      read_bytes = read(pipe_read_fd, &next_num, sizeof(next_num));
    }
    close(p[1]);
    wait(0);
    exit(0);
  }

  return;
}
