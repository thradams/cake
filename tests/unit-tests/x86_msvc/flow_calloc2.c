/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    int i;
    void * p;
};

void *calloc(int i, int sz);
void free(void * p);

int main()
{
    struct X * p;

    p = calloc(1, 1);
    if (p)
    {
    }
    free(p);
}
