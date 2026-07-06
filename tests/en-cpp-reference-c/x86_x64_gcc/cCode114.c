/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(int argc, char *argv[])
{
    printf("argc = %d\n", argc);
    {
        int ndx;

        ndx = 0;
        for (; ndx != argc; ++ndx)
        printf("argv[%d] --> %s\n", ndx, argv[ndx]);
    }
    printf("argv[argc] = %p\n", (void *)argv[argc]);
}


