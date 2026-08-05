#pragma safety enable


int* _Owner  get();

void f()
{
    int* _Owner _Opt p = 0; // owner object (p) not moved (root see line 9)
    p = get();
} //lint 29 owner object (p) not moved (root see line 9)
