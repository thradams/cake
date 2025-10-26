/* Cake 0.12.26 x86_msvc */

void *malloc(int size);
void free(void * ptr);

void f(int condition)
{
    int * p;

    p = malloc(4);
    if (condition)
    {
        goto end;
    }
    free(p);
    end:
}


