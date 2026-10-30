
#pragma safety enable


int* _Nonnull _Owner  get();
void dtor(int* _Owner p);

void f(int a)
{
    int* _Owner p = 0;
    p = get();    
    dtor(p);    
}
