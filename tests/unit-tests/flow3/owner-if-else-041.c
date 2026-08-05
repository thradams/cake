#pragma safety enable

void* _Owner _Opt _Uninitialized malloc(int sz);

void f(int i)
{
    if (i)
    {
    }
    else
    {
        int* _Owner _Opt p3 = malloc(1); // owner object (p3) not moved (call-opt-nonnull see line 12)
    } //lint 29 owner object (p3) not moved (call-opt-nonnull see line 12)
}
