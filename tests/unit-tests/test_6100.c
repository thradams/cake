
#pragma nullable enable
#pragma ownership enable

int* _Owner  get();
void dtor(int* _Owner p);

void f(int a)
{
    int* _Owner _Opt p = 0;
    p = get();    
    dtor(p);    
}
