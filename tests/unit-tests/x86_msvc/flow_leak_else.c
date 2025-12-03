/* Cake x86_msvc */

void *malloc(int size);
void free(void * ptr);

void f(int c)
{
    int * p;

    p = malloc(4);
    if (c)
    {
        free(p);
    }
}


