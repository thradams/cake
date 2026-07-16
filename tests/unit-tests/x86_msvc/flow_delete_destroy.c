/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * text;
};

void free(void * ptr);

void x_destroy(struct X * p)
{
    free(p->text);
}

void x_delete(struct X * p)
{
    if (p)
    {
        x_destroy(p);
        free(p);
    }
}
