/* Cake x64_msvc */

void *malloc(int size);
void free(void * ptr);

void f(int condition)
{
    int * p;

    p = malloc(4ULL);
    if (condition)
    {
        goto end;
    }
    free(p);
    end:
}
