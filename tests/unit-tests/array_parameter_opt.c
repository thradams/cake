#pragma safety enable

// int a[_Nullable 2] is int* _Nullable a (6.7.6.3p7): null can be passed

void f1(int a[_Nullable 2]);
void f1(int * _Nullable a);

void f2(int a[2]);

void f3(void)
{
    f1(nullptr);
    f1(0);
    f2(nullptr); //lint 39 passing null as array
    f2(0); //lint 39
}

void f4(int a[_Nullable 2])
{
    static_assert(_Generic(&a, int* _Nullable*: 1, default: 0));
}
