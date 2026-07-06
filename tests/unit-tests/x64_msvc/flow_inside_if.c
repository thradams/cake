/* Cake x64_msvc */

struct X {
    char * name;
};

void *malloc(int i);
void free(void * p);

int main()
{
    struct X * p;

    p = malloc(8ULL);
    if (p)
    {
        p->name = malloc(10);
        free(p->name);
    }
    free(p);
}
