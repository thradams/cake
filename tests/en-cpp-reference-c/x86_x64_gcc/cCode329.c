/* Cake x86_x64_gcc */

extern char *strchr(char * __s, int __c);
extern int printf(char * __format, ...);

int main(void)
{
    char * str;
    char target;
    char * result;

    str = "Try not. Do, or do not. There is no try.";
    target = 84;
    result = str;
    while ((result = strchr(result, target)) != 0)
    {
        printf("Found '%c' starting at '%s'\n", target, result);
        ++result;
    }
}


