/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

void *malloc(unsigned long size);
void free(void * ptr);

void f1()
{
    void * p;

    p = malloc(1);
    if (p)
    {
    }
    free(p);
}

void f2(int condition)
{
    void * p;

    p = malloc(1);
    if (condition)
    {
    }
}

void f3(int condition)
{
    void * p;

    p = malloc(1);
    if (condition)
    {
        free(p);
    }
    else
    {
        free(p);
    }
}

void f4(int condition)
{
    void * p;

    p = malloc(1);
    if (condition)
    {
    }
    else
    {
        free(p);
    }
}

void f5(int condition)
{
    void * p;

    p = malloc(1);
    if (condition)
    {
        free(p);
    }
    else
    {
    }
}

void f6(int condition)
{
    void * p;

    p = malloc(1);
    if (p)
    {
        free(p);
        return;
    }
}
