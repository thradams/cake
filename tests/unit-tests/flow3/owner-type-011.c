#pragma safety  enable


void f(int* _Owner p2)
{
    int* _Owner p1 = p2; // owner object (p1) not moved (root see line 4)
} //lint 29 owner object (p1) not moved (root see line 4)
