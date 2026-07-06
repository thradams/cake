/* Cake x86_x64_gcc */

extern int strcmp(char * __s1, char * __s2);
extern int printf(char * __format, ...);

void demo(char * lhs, char * rhs)
{
    int rc;
    char * rel;

    rc = strcmp(lhs, rhs);
    rel = rc < 0 ? "precedes" : rc > 0 ? "follows" : "equals";
    printf("[%s] %s [%s]\n", lhs, rel, rhs);
}


int main(void)
{
    char * string;

    string = "Hello World!";
    demo(string, "Hello!");
    demo(string, "Hello");
    demo(string, "Hello there");
    demo("Hello, everybody!" + 12, "Hello, somebody!" + 11);
}


