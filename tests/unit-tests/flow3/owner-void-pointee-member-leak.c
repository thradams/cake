#pragma safety enable

/* void* _Owner without _Clear: the pointee gets the struct type (ANY), so its members are checked */
void* _Owner calloc(unsigned long n, unsigned long s);
void free(void* _Owner _Opt p);

struct X { int* _Owner p; };

void f(void)
{
    struct X* _Owner x = calloc(1, sizeof(struct X));
    if (!x) return;
    x->p = calloc(1, sizeof(int)); //lint 26
    free(x); //lint 29
}
