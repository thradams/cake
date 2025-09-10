#pragma safety enable


char* _Owner strdup(const char* s);
void* _Owner malloc(unsigned size);

void free(void* _Owner _Opt ptr);

struct X {
    char* _Owner _Opt name;
};

void x_destroy(_Dtor struct X* p)
{
    free(p->name);
}

void x_print(struct X* p)
{
    //printf("%s", p->name);
}

int main()
{
    //warning: object pointed by 'x.name' was not released.
    struct X x [[cake::leak]] = {0};
    x.name = strdup("a");
    x_destroy(&x);

    //warning: uninitialized object '&x.name'
    [[cake::w30]]
    x_print(&x);

}


