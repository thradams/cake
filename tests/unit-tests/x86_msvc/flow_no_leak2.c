/* Cake x86_msvc */

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


