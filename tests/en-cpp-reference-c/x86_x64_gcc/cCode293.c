/* Cake x86_x64_gcc */

extern void (*signal(int __sig, void (* __handler)(int)))(int);
extern int raise(int __sig);
extern int printf(char * __format, ...);

int main(void)
{
    signal(15, ((void (*)(int))1));
    raise(15);
    printf("Exit main()\n");
}


