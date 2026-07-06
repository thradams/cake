/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    printf("%d\n", 0);
    printf("%zu  %zu\n", 4L, 4L);
    printf("%zu  %zu\n", 8L, 8L);
    return 0;
}


