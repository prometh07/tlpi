#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/time.h> 

// Write a program that measures the bandwidth provided by pipes. As command-
// line arguments, the program should accept the number of data blocks to be sent
// and the size of each data block. After creating a pipe, the program splits into two
// process: a child that writes the data blocks to the pipe as fast as possible, and a
// parent that reads the data blocks. After all data has been read, the parent should
// print the elapsed time required and the bandwidth (bytes transferred per second).
// Measure the bandwidth for different data block sizes.

#define try(condition) if(condition) {} else goto rescue;

int str_to_int(char* str) {
  char* p = NULL;
  long conv = strtol(str, &p, 10);
  if (errno == ERANGE || errno == EINVAL || conv > INT_MAX || *p != '\0') {
    perror(NULL);
    exit(1);
  }
  return (int)conv;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("Usage: %s number_of_blocks size_of_block (real block size is multiplied by sizeof(char))\n", argv[0]);
    return 1;
  }

  int number_of_blocks = str_to_int(argv[1]);
  int size_of_block = str_to_int(argv[2]);

  int pipe_file_descriptors[2];
  char* buffer = NULL;
  try( (buffer = malloc(size_of_block * sizeof(char))) != NULL )
  try( pipe(pipe_file_descriptors) != -1)
  
  pid_t pid;
  try( (pid = fork()) >= 0 )
  if (pid > 0) {
    try( close(pipe_file_descriptors[0]) != -1 )
    int i = 0;
    for (i = 0; i < number_of_blocks; i++) {
      try( write(pipe_file_descriptors[1], buffer, sizeof(buffer)) != -1 )
    }
    try( close(pipe_file_descriptors[1]) != -1 )
  }
  else if (pid == 0) {
    try( close(pipe_file_descriptors[1]) != -1 )
    int bytes_read = 0;
    int blocks_read = 0;
    struct timeval t1, t2;
    double elapsed_time;
    
    gettimeofday(&t1, NULL);
    while (true) {
      bytes_read = read(pipe_file_descriptors[0], buffer, sizeof(buffer));
      try( bytes_read != -1)
      if (bytes_read == 0) {
        break;
      }
      blocks_read++;
    }
    gettimeofday(&t2, NULL);
    if (blocks_read == number_of_blocks) {
      printf("Read all blocks\n");
    }
    else {
      printf("Number of read blocks is different than given argument. Some error occured.\n");
    }
    // compute and print the elapsed time in millisec (from so)
    elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    printf("Reading took %f ms.\n", elapsed_time);
  }

  free(buffer);
  return 0;
rescue:
  perror(argv[0]);
  free(buffer);
  return 1;
}
