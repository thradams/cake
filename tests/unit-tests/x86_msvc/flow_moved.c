/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

void free(void * p);

void f(int * p)
{
    int * p2;

    p2 = p;
    if (p)
    {
    }
    free(p2);
}
