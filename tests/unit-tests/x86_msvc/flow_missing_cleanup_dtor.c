/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * name;
};

void free(void * p);

void x_delete(struct X * p)
{
    if (p)
    {
        free(p);
    }
}
