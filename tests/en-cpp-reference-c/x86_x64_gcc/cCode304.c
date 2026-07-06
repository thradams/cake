/* Cake x86_x64_gcc */

volatile int gSignalStatus = 0;
void signal_handler(int status)
{
    gSignalStatus = status;
}


extern void (*signal(int __sig, void (* __handler)(int)))(int);
extern int printf(char * __format, ...);
extern int raise(int __sig);

int main(void)
{
    signal(2, signal_handler);
    printf("SignalValue:    %d\n", gSignalStatus);
    printf("Sending signal: %d\n", 2);
    raise(2);
    printf("SignalValue:    %d\n", gSignalStatus);
}


