//en.cppreference.com/w/c/program/sig_atomic_t.html
#include <signal.h>
#include <stdio.h>
 
volatile sig_atomic_t gSignalStatus = 0;
 
void signal_handler(int status)
{
    gSignalStatus = status;
}
 
int main(void)
{
    /* Install a signal handler. */
    signal(SIGINT, signal_handler);
 
    printf("SignalValue:    %d\n", gSignalStatus);
    printf("Sending signal: %d\n", SIGINT);
    raise(SIGINT);
    printf("SignalValue:    %d\n", gSignalStatus);
}