/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int abs(int __x);

int main(void)
{
    printf("abs(+3) = %d\n", abs(3));
    printf("abs(-3) = %d\n", abs(-3));
}


