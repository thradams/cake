void f(int a[3]);
void g(void)
{
    int b[2] = { 0 };
    f(b); //lint 1130 argument array of size 2 is smaller than the parameter array size 3
}
