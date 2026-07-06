/* Cake x86_x64_gcc */

extern int raise(int __sig);
extern int printf(char * __format, ...);

int main(void)
{
    raise(15);
    printf("Exit main()\n");
}


