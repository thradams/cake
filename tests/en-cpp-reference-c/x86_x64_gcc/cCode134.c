/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    int n;
    int * p;

    n = 1;
    p = &n;
    printf("*p = %d\n", *p);
    *p = 7;
    printf("*p = %d\n", *p);
}


