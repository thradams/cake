/* Cake x64_msvc */

void *malloc(int size);
void free(void * ptr);

void f(int c)
{
    int * p;

    p = malloc(4ULL);
    if (c)
    {
        free(p);
    }
}
