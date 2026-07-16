/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * name;
};

struct Y {
    struct X x;
};

void x_destroy(struct X * p);

void y_destroy(struct Y * p)
{
    x_destroy(&p->x);
}
