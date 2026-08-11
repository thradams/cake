#pragma safety enable


char* _Owner strdup(const char* s);
void* _Owner _Uninitialized malloc(unsigned size);

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
    struct X x = {0}; // owner object (.name) not moved (root see line 27)
    x.name = strdup("a");
    x_destroy(&x);

    //warning: uninitialized object '&x.name'
    x_print(&x); //lint 30 possible uninitialized object '*&x' (name is uninitialized after x_destroy)

} //lint 29 owner object (.name) not moved (root see line 31)
