#pragma safety enable

void* _Owner malloc(unsigned long n);
void free(void* _Owner _Nullable p);

int* gq;

void f(void)
{
    int* _Owner p = malloc(sizeof(int));
    if (!p) return;
    gq = p;
    free(p);
} //lint 31
