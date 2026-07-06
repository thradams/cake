/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
static int __c0_count = 0;


void f(void)
{
    int i;

    i = 0;
    printf("%d %d\n", i++, __c0_count++);
}


int main(void)
{
    {
        int ndx;

        ndx = 0;
        for (; ndx < 10; ++ndx)
        f();
    }
}


