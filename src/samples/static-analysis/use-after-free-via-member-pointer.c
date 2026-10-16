#pragma safety enable
void* _Owner _Opt malloc(unsigned long n);
void free(void* _Owner _Opt p);

struct S { int a; };

int f(void)
{
    struct S* _Owner _Opt s = malloc(sizeof(struct S));
    if (!s) return 0;
    s->a = 1;
    int* q = &s->a;
    free(s);
    return *q;   // use after free
}
