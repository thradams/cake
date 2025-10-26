/* Cake 0.12.26 x86_msvc */
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


