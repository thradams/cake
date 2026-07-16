/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * name;
};

struct Y {
    struct X x;
};

void free(void * ptr);

void f(struct Y * y, struct X * p)
{
    free(y->x.name);
    y->x = *p;
}
