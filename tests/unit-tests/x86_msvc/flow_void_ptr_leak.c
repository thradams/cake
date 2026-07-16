/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

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
