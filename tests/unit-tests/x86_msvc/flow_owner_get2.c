/* Cake 0.14.05 x86_msvc */

int *get();
void dtor(int * p);

void f(int a)
{
    int * p;

    p = 0;
    p = get();
    dtor(p);
}
