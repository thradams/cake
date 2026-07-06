/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    printf("the first element is at offset %zu\n", 0L);
    printf("the double is at offset %zu\n", 8L);
}


