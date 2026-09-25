void f(void)
{
    const int i = 0;
    i++; //lint 920 increment of read-only object 'i'
    ++i; //lint 920 increment of read-only object 'i'
}
