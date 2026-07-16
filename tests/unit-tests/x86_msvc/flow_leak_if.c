/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

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
