/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

void signal_handler(int sig)
{
    printf("Received signal: %d\n", sig);
}


extern void (*signal(int __sig, void (* __handler)(int)))(int);
extern void exit(int __status);
extern int raise(int __sig);

int main(void)
{
    if (signal(15, signal_handler) == ((void (*)(int))-1))
    {
        printf("Error while installing a signal handler.\n");
        exit(1);
    }
    printf("Sending signal: %d\n", 15);
    if (raise(15) != 0)
    {
        printf("Error while raising the SIGTERM signal.\n");
        exit(1);
    }
    printf("Exit main()\n");
    return 0;
}


