/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * p;
    char * p2;
};

char *strdup(char * s);
void free(void * p);

int main()
{
    char * p0;
    struct X x;

    p0 = strdup("a");
    x.p = p0;
    x.p2 = 0;
    free(x.p);
}
