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
        /* FIXED: this used to ALSO warn about '(*p).name' (null-pointer
           and moved-object checks) here -- free's own parameter is
           void*, so it never accesses *p's structure at all; passing p
           to it is just consuming p itself. */
        free(p);
    }
}

int main()
{
    struct X* _Owner _Opt p = malloc(sizeof * p);

    //warning: uninitialized object 'p.name'

    x_delete(p); //lint 30 passing a possible uninitialized object '(*p)' -- malloc is _Uninitialized
}
