#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define try(condition) if(condition) {} else goto rescue;

int main(int argc, char** argv) {
  sigset_t signal_set;
  sigset_t previous_set;

  try(sigemptyset(&signal_set) != -1)
  try(sigaddset(&signal_set, SIGINT) != -1)

  try(sigprocmask(SIG_BLOCK, &signal_set, &previous_set) >= 0)
  try(raise(SIGINT) != -1)
  try(sigpending(&signal_set) != -1)
  printf(sigismember(&signal_set, SIGINT) ? "SIGINT pending\n" : "SIGINT not pending\n");
  
  struct sigaction ignore_action;
  ignore_action.sa_handler = SIG_IGN;
  if (argc >= 2 && strcmp(argv[1], "ignore") == 0) {
    try(sigaction(SIGINT, &ignore_action, NULL) != -1)
  }

  // If you set the action for a signal to SIG_IGN, 
  // or if you set it to SIG_DFL and the default action is to ignore that signal, 
  // then any pending signals of that type are discarded (even if they are blocked) 
  try(sigpending(&signal_set) != -1)
  printf(sigismember(&signal_set, SIGINT) ? "SIGINT still pending\n" : "SIGINT already not pending\n");

  try(sigprocmask(SIG_SETMASK, &previous_set, NULL) >= 0)
  return 0;
rescue:
  perror(argv[0]);
  return 1;
}
