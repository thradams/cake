#pragma safety enable

_Uninitialized void* _Owner _Opt malloc(int sz);

void f(int i)
{
    if (i)
    {
    }
    else
    {
        int* _Owner _Opt p3 = malloc(sizeof * p3); // owner object (p3) not moved (call-opt-nonnull see line 12)
    } //lint 29 owner object (p3) not moved (call-opt-nonnull see line 12)
}
