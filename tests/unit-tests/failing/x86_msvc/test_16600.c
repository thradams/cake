/* Cake x86_msvc */
struct X {
    char * name;
};


void *calloc(int n, int i);
void free(void * p);

int main()
{
    struct X * p;

    p = calloc(1, 4);
    if (p)
    {
        p->name = calloc(1, 2);
    }
    if (p)
    {
        free(p->name);
    }
    free(p);
}


