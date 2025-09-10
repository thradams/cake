#pragma safety enable


void* _Owner _Opt malloc(int i);
void free(void* _Owner _Opt p);

struct X {
    char* _Owner name;
};

void x_delete(struct X* _Owner _Opt p)
{
    if (p)
    {
        free(p->name);
        free(p);
    }
}

int main()
{
    struct X* _Owner _Opt p = malloc(sizeof * p);

    //warning: uninitialized object 'p.name'
    [[cake::W30]]
    x_delete(p);
}


