/* Cake x86_msvc */

void *malloc(unsigned long size);
void free(void * ptr);

int main()
{
    void * p;

    p = malloc(1);
    free(p);
}


