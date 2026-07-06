/* Cake x86_x64_gcc */

extern int vsscanf(char * __s, char * __format, __builtin_va_list __arg);

unsigned char checked_sscanf(int count, char * buf, char * fmt, ...)
{
    __builtin_va_list ap;
    int rc;

    __builtin_va_start(ap, fmt);
    rc = vsscanf(buf, fmt, ap);
    __builtin_va_end(ap);
    return rc == count;
}


extern int printf(char * __format, ...);
extern int puts(char * __s);

int main(void)
{
    int n;
    int m;

    printf("Parsing '1 2'...");
    if (checked_sscanf(2, "1 2", "%d %d", &n, &m))
    {
        puts("success");
    }
    else
    {
        puts("failure");
    }
    printf("Parsing '1 a'...");
    if (checked_sscanf(2, "1 a", "%d %d", &n, &m))
    {
        puts("success");
    }
    else
    {
        puts("failure");
    }
}


