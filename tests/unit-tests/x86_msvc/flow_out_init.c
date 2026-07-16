/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * s;
};

char *strdup(char * s);

void init(struct X * px)
{
    px->s = strdup("a");
}

void free(void * p);

int main()
{
    struct X x;

    init(&x);
    free(x.s);
}
