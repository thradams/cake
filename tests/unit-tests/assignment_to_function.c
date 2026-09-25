void g(void);
void f(void)
{
    g = 0; //lint 900 cannot assign to a function
}
