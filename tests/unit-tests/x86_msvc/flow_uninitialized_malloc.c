/* Cake 0.12.26 x86_msvc */
struct X {
    char * name;
};


void free(void * p);

void x_delete(struct X * p)
{
    if (p)
    {
        free(p->name);
        free(p);
    }
}


void *malloc(int i);

int main()
{
    struct X * p;

    p = malloc(4);
    x_delete(p);
}


