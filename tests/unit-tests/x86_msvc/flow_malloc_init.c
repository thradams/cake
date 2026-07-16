/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    int i;
    void * p;
};

void *malloc(int i, int sz);
void free(void * p);

int main()
{
    struct X * p;

    p = malloc(1, 1);
    if (p)
    {
    }
    free(p);
}
