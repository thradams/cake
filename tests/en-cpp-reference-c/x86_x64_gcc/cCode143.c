/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    short x;

    printf("sizeof(float)          = %zu\n", 4L);
    printf("sizeof(void(*)(void))  = %zu\n", 8L);
    printf("sizeof(char[10])       = %zu\n", 10L);
    printf("sizeof 'a'             = %zu\n", 4L);
    printf("sizeof &main           = %zu\n", 8L);
    printf("sizeof \"hello\"         = %zu\n", 6L);
    printf("sizeof x               = %zu\n", 2L);
    printf("sizeof (x+1)           = %zu\n", 4L);
}


