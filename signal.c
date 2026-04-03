#include <signal.h>
#include <unistd.h>

void sigint(int sig) {
  (void)sig;
  const char msg[] = "\nCaught SIGINT\n";
  write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

void handle_sigint() {

  struct sigaction sa;
  sa.sa_handler = sigint;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  sigaction(SIGINT, &sa, NULL);


}