/* Cake 0.14.05 x86_msvc */

struct X {
    char * text;
};

void x_destroy(struct X * p);

void x_delete(struct X * p)
{
    if (p)
    {
        x_destroy(p);
    }
}
