/* Cake 0.12.26 x86_msvc */

int *get();
void dtor(int * p);

void f(int a)
{
    int * p;

    p = 0;
    p = get();
    dtor(p);
}


