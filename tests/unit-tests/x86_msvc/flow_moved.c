/* Cake 0.12.26 x86_msvc */

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


