#pragma safety enable

void f(int i)
{
    int j;

    if (i || j) // warning: using a uninitialized object 'j'
    {
    }
}
