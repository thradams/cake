#pragma safety enable


struct S {
    char* _Nonnull _Owner p;
};

void swap(struct S* a, struct S* b)
{
    struct S temp = *a;
    *a = *b;
    *b = temp;
}
