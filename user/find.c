#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *name);

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("usage: find [directory] [name]\n");
    exit(1);
  }

  find(argv[1], argv[2]);

  exit(0);
}

void find(char *path, char *name) {
  // open directory
  int fd = open(path, 0);
  if (fd < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    close(fd);
    return;
  }

  struct stat st;
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  if (st.type != T_DIR) {
    close(fd);
    return;
  }

  // get file under path
  struct dirent child_de;
  while (read(fd, &child_de, sizeof(child_de)) == sizeof(child_de)) {
    if (child_de.inum == 0) {
      continue;
    }
    if (strcmp(child_de.name, ".") == 0 || strcmp(child_de.name, "..") == 0) {
      continue;
    }

    // compare name
    if (strcmp(name, child_de.name) == 0) {
      printf("%s/%s\n", path, child_de.name);
    }

    // recursively call find()
    char new_path[512];
    char *ptr;
    strcpy(new_path, path);
    ptr = new_path + strlen(path);
    *ptr = '/'; 
    ptr++;
    strcpy(ptr, child_de.name);

    find(new_path, name);
  }

  close(fd);
}
