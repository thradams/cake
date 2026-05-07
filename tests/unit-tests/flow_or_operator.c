#pragma safety enable

void f(int i)
{
    int j;

    if (i || j) //lint 30 warning: using a uninitialized object 'j'
    {
    }
}