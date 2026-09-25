void f(int a[2], int b[2])
{
    a = b; //lint 43 assignment to array parameter 'a', which is a pointer
    (void)a;
}
