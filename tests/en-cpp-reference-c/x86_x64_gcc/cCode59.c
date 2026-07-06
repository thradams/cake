/* Cake x86_x64_gcc */

extern int puts(char * __s);
extern void perror(char * __s);

int main(void)
{
    int rc;

    rc = puts("Hello World");
    if (rc == -1)
    {
        perror("puts()");
    }
}


