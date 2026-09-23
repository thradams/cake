#pragma safety enable

void * _Opt _Owner _Clear calloc(unsigned long n, unsigned long s);
char * _Opt _Owner strdup(const char* s);
void free(void * _Opt _Owner p);

struct X {
    char * _Owner name;
};

struct Y {
    struct X x;
    struct X * px;
};

void x_destroy(_Dtor struct X * p)
{
    free(p->name);
}

void f(struct Y * p)
{
    x_destroy(&p->x);
    x_destroy(p->px);
}

int main() {
    struct Y y = {};
    y.x.name = strdup("a");
    f(&y);
    free(y.x.name);
}
