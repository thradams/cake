/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

void signal_handler(int signal)
{
    printf("Received signal %d\n", signal);
}


extern void (*signal(int __sig, void (* __handler)(int)))(int);
extern int raise(int __sig);

int main(void)
{
    signal(15, signal_handler);
    printf("Sending signal %d\n", 15);
    raise(15);
    printf("Exit main()\n");
}


