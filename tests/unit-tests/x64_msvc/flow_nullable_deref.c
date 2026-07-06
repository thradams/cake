/* Cake x64_msvc */

struct X {
    char * name;
};

void *malloc(int i);
void free(void *);

int main()
{
    struct X * p;

    p = malloc(8ULL);
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
