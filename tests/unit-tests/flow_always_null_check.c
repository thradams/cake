#pragma safety enable

void f(int* p)
{
    //pointer is always non-null   
    [[cake::w28]]
    if (p != 0) {}

    //pointer is always non-null
    [[cake::w28]]
    if (0 != p) {}

    //pointer is always non-null
    [[cake::w28]]
    if (p == 0) {}

    //pointer is always non-null
    [[cake::w28]]
    if (0 == p) {}
}
