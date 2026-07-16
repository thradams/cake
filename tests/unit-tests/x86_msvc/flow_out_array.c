/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

void f(char s[2])
{
}

void test1()
{
    char s[2];

    f(s);
}

void f2(char * s)
{
    *s = 0;
}

void test2()
{
    char s[2];

    f2(s);
}

int main()
{
}
