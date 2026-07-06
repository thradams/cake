/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    printf("%d %d %d\n", 0, 1, 1);
    printf("%d %d\n", 0, 2);
    printf("%zu\n", 2L);
}


