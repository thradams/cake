#pragma safety enable



void free(void *_Owner _Nullable ptr);
void *_Owner _Nullable malloc(int size);
char *_Owner _Nullable strdup(const char *);

struct X
{
    char *_Owner _Nullable name;
};

struct Y
{
    struct X x;
    struct X * _Nullable px;
};

void x_destroy(_Dtor struct X * _Nullable p)
{
    if (p)
     free(p->name);
}

void f(struct Y *p)
{
    x_destroy(p->px);
#pragma cake diagnostic check "-Wmust-use-address-of"
}

int main()
{
    struct Y y = {};
    struct X *p = &y.x;
    x_destroy(&y.x);
}
