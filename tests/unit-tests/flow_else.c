#pragma safety enable

void* _Owner _Opt malloc(int sz);

void f(int i)
{
    if (i)
    {
    }
    else
    {
        int* _Owner _Opt p3 [[cake::leak]] = malloc(1);
    }
}

