/* Cake 0.12.26 x86_msvc */
struct X {
    char * name;
};


void *malloc(int i);
void free(void * p);

int main()
{
    struct X * p;

    p = malloc(4);
    if (p)
    {
        p->name = malloc(10);
        free(p->name);
    }
    free(p);
}


