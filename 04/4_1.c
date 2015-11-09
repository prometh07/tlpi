#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096

char* program_name;

void errno_error() {
  perror(program_name);
  exit(1);
}

mode_t get_append_mode(int argc, char** argv) {
  int c;
  mode_t append_mode = 0;
  while((c = getopt(argc, argv, "a")) != -1) {
    switch(c) {
      case 'a':
        append_mode = O_APPEND;
        break;
      default:
        exit(1);
    }
  }
  return append_mode;
}

int* open_files(int argc, char** argv, int* file_descriptors, mode_t append_mode) {
  mode_t permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
  int i, j;
  if((file_descriptors = malloc((argc - optind) * sizeof(int*))) == NULL)
    errno_error();
  for (i = optind, j=0; i < argc; i++, j++)
    if((file_descriptors[j] = open(argv[i], O_WRONLY | O_CREAT | append_mode, permissions)) == -1)
      errno_error();
  return file_descriptors;
}

void close_files(int *file_descriptors, int number_of_files) {
  int i ;
  for(i = 0; i < number_of_files; i++)
    if(close(file_descriptors[i]) == -1)
      errno_error();
}

int main(int argc, char **argv) {
  program_name = strdup(argv[0]);
  mode_t append_mode = get_append_mode(argc, argv);
  int number_of_files = argc - optind;
  int* file_descriptors = NULL;
  if(number_of_files > 0)
    file_descriptors = open_files(argc, argv, file_descriptors, append_mode);

  char buffer[BUFFER_SIZE];
  ssize_t bytes_read;
  
  while((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE))) {
    if(bytes_read == -1)
      errno_error();
    if(write(STDOUT_FILENO, buffer, bytes_read) == -1)
      errno_error();
    int i;
    for(i = 0; i < number_of_files; i++)
      if((write(file_descriptors[i], buffer, bytes_read)) == -1)
        errno_error();
  }

  close_files(file_descriptors, number_of_files);
  free(file_descriptors);
  free(program_name);
  return 0;
}
