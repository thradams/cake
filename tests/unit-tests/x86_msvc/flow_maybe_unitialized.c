/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * text;
};

void *malloc(unsigned long size);
void x_delete(struct X * p);

int main()
{
    struct X * p;

    p = malloc(4);
    x_delete(p);
}
