/* Cake x86_x64_gcc */

extern int strncmp(char * __s1, char * __s2, unsigned int long __n);
extern int printf(char * __format, ...);

void demo(char * lhs, char * rhs, int sz)
{
    int rc;

    rc = strncmp(lhs, rhs, sz);
    if (rc < 0)
    {
        printf("First %d chars of [%s] precede [%s]\n", sz, lhs, rhs);
    }
    else
    {
        if (rc > 0)
        {
            printf("First %d chars of [%s] follow [%s]\n", sz, lhs, rhs);
        }
        else
        {
            printf("First %d chars of [%s] equal [%s]\n", sz, lhs, rhs);
        }
    }
}


int main(void)
{
    char * string;

    string = "Hello World!";
    demo(string, "Hello!", 5);
    demo(string, "Hello", 10);
    demo(string, "Hello there", 10);
    demo("Hello, everybody!" + 12, "Hello, somebody!" + 11, 5);
}


