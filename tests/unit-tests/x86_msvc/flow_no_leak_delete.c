/* Cake 0.14.05 x86_msvc */

struct X {
    char * text;
};

void free(void * p);

void x_delete(struct X * p)
{
    if (p)
    {
        free(p->text);
        free(p);
    }
}
