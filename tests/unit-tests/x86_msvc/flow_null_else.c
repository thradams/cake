/* Cake 0.12.26 x86_msvc */

void *malloc(unsigned long size);
void free(void * ptr);

void f5()
{
    void * p;

    p = malloc(1);
    if (p)
    {
        free(p);
        return;
    }
}


