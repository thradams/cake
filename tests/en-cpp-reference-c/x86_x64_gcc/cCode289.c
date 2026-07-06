/* Cake x86_x64_gcc */

int fun_quadruple(int x)
{
    return 4 * x;
}


int fun_double(int x)
{
    return 2 * x;
}


extern int printf(char * __format, ...);
extern int puts(char * __s);

int main(void)
{
    printf("quadruple(13): %d\n", fun_quadruple(13));
    printf("double(21): %d\n", fun_double(21));
    printf("%d\n", 34);
    puts("billion");
}


