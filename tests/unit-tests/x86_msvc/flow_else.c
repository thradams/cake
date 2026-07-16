/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

void *malloc(int sz);

void f(int i)
{
    if (i)
    {
    }
    else
    {
        int * p3;

        p3 = malloc(1);
    }
}
