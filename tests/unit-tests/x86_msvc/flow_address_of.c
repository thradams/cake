/* Cake 0.12.26 x86_msvc */
struct X {
    char * p;
};


void x_destroy(struct X * p);

void f(struct X * x)
{
    x_destroy(x);
}


