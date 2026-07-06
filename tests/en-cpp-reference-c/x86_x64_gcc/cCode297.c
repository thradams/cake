/* Cake x86_x64_gcc */

extern int puts(char * __s);

void f1(void)
{
    puts("f1");
}


void f2(void)
{
    puts("f2");
}


extern int atexit(void (* __func)(void));

int main(void)
{
    if (!atexit(f1) && !atexit(f2) && !atexit(f2))
    {
        return 0;
    }
    return 1;
}


