#pragma safety enable

void free(void* _Owner _Opt p);
void* _Owner _Opt malloc(unsigned long n);

struct X {
    int * _Opt p;
};

struct X f(struct X* pX)
{
    struct X x;

    int a = 0;
    x.p = &a;
    pX->p = &a;
    return x; //lint 31 31
}

int* f2(void)
{
    int a = 0;
    int* p = &a;
    return p; //lint 31
}

void f3(struct X* pX)
{
    {
        int a = 0;
        pX->p = &a;
    }
} //lint 31

void f4(struct X* pX)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    if (p == 0) return;
    pX->p = p;
    free(p);
} //lint 31

/* no warnings */
int* g1(int* p)
{
    int a = 0;
    int* q = &a;
    *q = 1;
    return p;
}

void g2(struct X* pX)
{
    struct X x2 = { 0 };
    int a = 0;
    pX = &x2;
    pX->p = &a;
}

int* g3(struct X* pX)
{
    static int s = 0;
    pX->p = &s;
    return &s;
}

void g4(struct X* pX)
{
    int a = 0;
    pX->p = &a;
    pX->p = 0;
}

struct Y {
    int i;
    struct Y* _Opt next;
};

void y_destroy(_Dtor struct Y* p);

void h1(struct Y* p)
{
    y_destroy(p);
} //lint 90

void h2(struct Y* p)
{
    if (p->next == 0) return;
    y_destroy(p->next);
} //lint 90

void h3(struct Y* p)
{
    static int s;
    y_destroy(p);
    p->i = s;
    p->next = 0;
}

void h4(_Dtor struct Y* p)
{
    y_destroy(p);
}

const char* i210(void)
{
    char buffer[100] = { 0 };
    return buffer; //lint 31
}

const char* i210b(void)
{
    char buffer[100] = { 0 };
    const char* p = buffer;
    return p; //lint 31
}

struct B {
    char* _Opt p;
};

void i210c(struct B* b)
{
    char buffer[100] = { 0 };
    b->p = buffer;
} //lint 31
