//en.cppreference.com/w/c/program/signal.html
#include <signal.h>
#include <stdio.h>
 
volatile sig_atomic_t gSignalStatus;
 
void signal_handler(int signal)
{
  gSignalStatus = signal;
}
 
int main(void)
{
  signal(SIGINT, signal_handler);
 
  printf("SignalValue: %d\n", gSignalStatus);
  printf("Sending signal: %d\n", SIGINT);
  raise(SIGINT);
  printf("SignalValue: %d\n", gSignalStatus);
}