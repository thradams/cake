/* a op= b reads a and b: the same warning 30 as a + b (A5.2) */
#pragma safety enable

void f(void)
{
    int a;
    a += 1; //lint 30
}

void g(int a)
{
    int b;
    a += b; //lint 30
}
