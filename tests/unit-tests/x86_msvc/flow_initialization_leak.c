/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * p;
};

char *strdup(char * s);

int main()
{
    struct X x;

    x.p = strdup("a");
}
