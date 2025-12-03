/* Cake x86_msvc */

void *calloc(unsigned long n, unsigned long size);
void free(void * ptr);

int main()
{
    int * p;
    int * p2;

    p = calloc(1, 4);
    p2 = p;
    free(p2);
    free(p);
}


