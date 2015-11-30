#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096
#define try(condition) if(condition) {} else goto rescue;

int main(int argc, char** argv) {
  if(argc != 3) {
    printf("Usage: %s from to\n", argv[0]);
    return 1;
  }

  int from_fd, to_fd;
  try((from_fd = open(argv[1], O_RDONLY)) != -1)
  try((to_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666)) != -1)

  char buffer[BUFFER_SIZE];
  ssize_t bytes_read;
  while((bytes_read = read(from_fd, buffer, BUFFER_SIZE)) > 0) {
    int from_index = 0;
    int hole_size = 0;
    int i = 0;
    for(i = 0; i < bytes_read; i++) {
      if(buffer == '\0') {
        if(hole_size == 0) {
          try(write(to_fd, buffer+from_index, i-from_index) != -1) // write data before the hole
        }
        hole_size++;
      }
      else if(hole_size > 0) {
        try(lseek(to_fd, hole_size, SEEK_CUR) != -1) // write hole
        hole_size = 0;
        from_index = i;
      }
    }
    try(lseek(to_fd, hole_size, SEEK_CUR) != -1) // write the last hole
    try(write(to_fd, buffer+from_index, i-from_index) != -1) // write data after the last hole
  }
  try(bytes_read != -1)
  try(close(from_fd) != -1)
  try(close(to_fd) != -1)

  return 0;
rescue:
  perror(argv[0]);
  return 1;
}
