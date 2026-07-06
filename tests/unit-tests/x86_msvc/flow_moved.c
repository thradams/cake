/* Cake 0.14.05 x86_msvc */

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
