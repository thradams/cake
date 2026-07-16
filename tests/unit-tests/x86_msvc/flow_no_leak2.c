/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

void *malloc(int size);
void free(void * ptr);

int main()
{
    int * p;

    p = malloc(4);
    if (p != 0)
    {
        free(p);
    }
}
