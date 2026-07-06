/* Cake 0.14.05 x86_msvc */

void *f();
void free(void * p);

int main()
{
    void * p;

    p = f();
    if (p)
    {
        free(p);
        p = f();
    }
}
