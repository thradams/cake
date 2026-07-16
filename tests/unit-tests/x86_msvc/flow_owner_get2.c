/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

int *get();
void dtor(int * p);

void f(int a)
{
    int * p;

    p = 0;
    p = get();
    dtor(p);
}
