#pragma safety enable

void* _Owner _Opt malloc(unsigned long n);
void free(void* _Owner _Opt p);

int* gq;

void f(void)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    if (!p) return;
    gq = p;
    free(p);
} //lint 31
