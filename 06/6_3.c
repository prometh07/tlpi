#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

extern char** environ;

int my_setenv(const char* name, const char* value, int overwrite) {
  if(name == NULL || name[0] == '\0' || strchr(name, '=') != NULL || value == NULL) {
    errno = EINVAL;
    return -1;
  }
  char* env = getenv(name);
  if((env && overwrite == 1) || !env) {
    char *result = malloc((strlen(name) + strlen(value) + 2) * sizeof(char));
    if(!result) {
      errno = ENOMEM;
      return -1;
    }
    if(sprintf(result, "%s=%s", name, value) < 0)
      return -1;
    return (putenv(result) == 0) ? 0 : -1;
  }
  return 0;
}

int my_unsetenv(const char* name) {
  if(name == NULL || name[0] == '\0' || strchr(name, '=') != NULL) {
    errno = EINVAL;
    return -1;
  }
  char** env;
  for (env = environ; *env != NULL; env++) {
    if(strstr(*env, name)) {
      char** last_element = env;
      while(*(last_element+1) != NULL) { ++last_element; } // find the last element of environ
      while(strstr(*last_element, name)) { *last_element-- = NULL; } // if we have multiple NAME=VAL definitions at the end of environ
      *env = *last_element;
      *last_element = NULL;
    }
  }
  return 0;
}

int main(int argc, char** argv) {
  my_setenv("test", "value", 0);
  printf("%s\n", getenv("test"));
  my_unsetenv("test");
  char **env;
  for (env = environ; *env != NULL; env++)
    puts(*env);
  return 0;
}
