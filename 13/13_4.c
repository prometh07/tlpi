#include <stdio.h>
#include <unistd.h>

// calling write immediately transfers data to kernel buffer cache
// calling printf doesn't do this until program terminates or stdout
// is redirected to terminal (if so, stdout is newline buffered, not
// fully buffered)
int main() {
  printf("If I had more time, \n");
  //fflush(stdout);
  write(STDOUT_FILENO, "I would have written you a shorter letter.\n", 43);
  return 0;
}
