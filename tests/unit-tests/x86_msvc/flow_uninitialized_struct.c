/* Cake 0.14.05 x86_msvc */

struct X {
    char * text;
};

void x_destroy(struct X * p);

int main()
{
    struct X x;

    x_destroy(&x);
}
