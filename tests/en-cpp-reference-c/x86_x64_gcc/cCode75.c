/* Cake x86_x64_gcc */

extern void exit(int __status);

void exit_now(int i)
{
    if (i > 0)
    {
        exit(i);
    }
}


extern int puts(char * __s);

int main(void)
{
    puts("Preparing to exit...");
    exit_now(2);
    puts("This code is never executed.");
}


