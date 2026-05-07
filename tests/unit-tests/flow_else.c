#pragma safety enable

void* _Owner _Opt malloc(int sz);

void f(int i)
{
    if (i)
    {
    }
    else
    {
        int* _Owner _Opt p3 = malloc(1);
    } //lint 29 memory leak
}

