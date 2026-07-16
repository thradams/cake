/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    char * name;
};

void *malloc(int i);
void free(void *);

int main()
{
    struct X * p;

    p = malloc(4);
    if (p)
    {
        p->name = malloc(1);
    }
    else
    {
    }
    free(p->name);
    free(p);
}
