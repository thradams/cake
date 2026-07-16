/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * p;
};

void x_destroy(struct X * p);

void f(struct X * x)
{
    x_destroy(x);
}
