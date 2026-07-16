/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

int *make1();
void free(void * p);
int *make2();

void f(int condition)
{
    int * p;

    p = 0;
    if (condition)
    {
        p = make1();
        free(p);
        p = make2();
    }
    else
    {
    }
    free(p);
}
