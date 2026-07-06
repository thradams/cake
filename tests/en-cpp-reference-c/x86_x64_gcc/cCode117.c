/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    printf("sizeof(struct S) = %zu\n", 2L);
    printf("alignof(struct S) = %zu\n", 1L);
    printf("sizeof(struct X) = %zu\n", 8L);
    printf("alignof(struct X) = %zu\n", 4L);
}


