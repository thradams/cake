/* Cake 0.12.26 x86_msvc */
struct X {
    int i;
    void * p;
};


void *malloc(int i, int sz);
void free(void * p);

int main()
{
    struct X * p;

    p = malloc(1, 1);
    if (p)
    {
    }
    free(p);
}


