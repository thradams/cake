/* Cake 0.12.26 x86_msvc */

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


