#pragma safety enable


void* _Owner _Opt _Uninitialized malloc(int i);
void free(void* _Owner _Opt p);

struct X {
    char* _Owner name;
};

void x_delete(struct X* _Owner _Opt p)
{
    if (p)
    {
        free(p->name);
        /* FIXED: no extra warnings about (*p).name, free takes void* and only consumes p */
        free(p);
    }
}

int main()
{
    struct X* _Owner _Opt p = malloc(sizeof * p);

    //warning: uninitialized object 'p.name'

    x_delete(p); //lint 30 passing a possible uninitialized object '(*p)' -- malloc is _Uninitialized
}
