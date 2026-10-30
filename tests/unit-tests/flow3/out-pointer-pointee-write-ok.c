#pragma safety enable


void construct(int* _Nonnull _Out* pp, int* v)
{
    *pp = v;
}

int main(int x)
{
    int* p;
    construct(&p, &x);
    return *p;
}
