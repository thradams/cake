/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

void *malloc(int sz);
void free(void * p);

void f(int i)
{
    void * p;

    p = 0;
    if (i)
    {
        if (i)
        {
            p = malloc(1);
        }
        else
        {
            p = malloc(1);
        }
    }
    free(p);
}
