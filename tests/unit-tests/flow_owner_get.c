#pragma safety enable


int* _Owner  get();

void f()
{
    int* _Owner _Opt p [[cake::leak]] = 0;
    p = get();
}


