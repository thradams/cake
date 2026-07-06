/* Cake x64_msvc */

void *malloc(int size);
void free(void * ptr);

int main()
{
    int * p;

    p = malloc(4ULL);
    if (p != 0)
    {
        free(p);
    }
}
