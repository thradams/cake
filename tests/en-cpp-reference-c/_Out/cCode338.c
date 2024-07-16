//en.cppreference.com/w/c/string/byte/strncmp.html
#include <stdio.h>
#include <string.h>
 
void demo(const char* lhs, const char* rhs, int sz)
{
    const int rc = strncmp(lhs, rhs, sz);
    if (rc < 0)
        printf("First %d chars of [%s] precede [%s]\n", sz, lhs, rhs);
    else if (rc > 0)
        printf("First %d chars of [%s] follow [%s]\n", sz, lhs, rhs);
    else
        printf("First %d chars of [%s] equal [%s]\n", sz, lhs, rhs);
}
int main(void)
{
    const char* string = "Hello World!";
    demo(string, "Hello!", 5);
    demo(string, "Hello", 10);
    demo(string, "Hello there", 10);
    demo("Hello, everybody!" + 12, "Hello, somebody!" + 11, 5);
}