#pragma safety enable

void f_const(const int a);
void f(int a);


int main(void)
{
    int a;

    //warning: passing an uninitialized argument 'a' object
    [[cake::w30]] [[cake::w30]]
        f_const(a);

    //warning: passing an uninitialized argument 'a' object
    [[cake::w30]] [[cake::w30]]
        f(a);

}