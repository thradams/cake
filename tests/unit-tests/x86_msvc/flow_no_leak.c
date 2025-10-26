/* Cake 0.12.26 x86_msvc */

void *malloc(int size);
void free(void * ptr);

int main()
{
    int * p;

    p = malloc(4);
    if (p)
    {
        free(p);
    }
}


