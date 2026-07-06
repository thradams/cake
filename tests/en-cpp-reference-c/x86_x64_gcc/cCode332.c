/* Cake x86_x64_gcc */

extern char *strcpy(char * __dest, char * __src);
extern int printf(char * __format, ...);

int main(void)
{
    char * src;
    char dst[];

    src = "Take the test.";
    strcpy(dst, src);
    dst[0] = 77;
    printf("src = %s\ndst = %s\n", src, dst);
}


