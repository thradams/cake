/* Cake 0.12.26 x86_msvc */
struct X {
    char * name;
};


void *malloc(int i);
void free(void *);

int main()
{
    struct X * p;

    p = malloc(4);
    if (p)
    {
        p->name = malloc(1);
    }
    else
    {
    }
    free(p->name);
    free(p);
}


