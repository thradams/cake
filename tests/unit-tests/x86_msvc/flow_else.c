/* Cake 0.14.05 x86_msvc */

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
