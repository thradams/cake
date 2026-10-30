#pragma safety enable
void free(void* _Owner _Opt p);
int* _Nonnull _Owner make(void);
int h1(void)
{
    int* _Owner p = make();
    int* q = p;
    free(p);
    return *q; //lint 31
}
