#pragma safety enable

void f(int i)
{
    int j;

    if (i || j) //lint 30
    {
    }
}
