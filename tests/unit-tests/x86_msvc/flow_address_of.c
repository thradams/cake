/* Cake 0.14.05 x86_msvc */

struct X {
    char * p;
};

void x_destroy(struct X * p);

void f(struct X * x)
{
    x_destroy(x);
}
