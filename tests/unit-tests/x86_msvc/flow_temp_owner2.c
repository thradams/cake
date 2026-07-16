/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

void *malloc(unsigned long size);
void free(void * ptr);

int main()
{
    void * p;

    p = malloc(1);
    free(p);
}
