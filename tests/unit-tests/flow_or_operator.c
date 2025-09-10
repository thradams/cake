#pragma safety enable

void f(int i)
{
    int j;
    // warning: using a uninitialized object 'j'
    [[cake::w30]]
    if (i || j)
    {
    }
}