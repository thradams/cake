#pragma safety enable
void* _Owner malloc(unsigned long n);
void free(void* _Owner _Nullable p);

struct S { int a; };

int f(void)
{
    struct S* _Owner s = malloc(sizeof(struct S));
    if (!s) return 0;
    s->a = 1;
    int* q = &s->a;
    free(s);
    return *q;   // use after free
}
