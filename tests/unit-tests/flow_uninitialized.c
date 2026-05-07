#pragma safety enable

void f_const(const int a);
void f(int a);


int main(void)
{
    int a;

    //warning: passing an uninitialized argument 'a' object

    f_const(a); //lint 30

    //warning: passing an uninitialized argument 'a' objec
    f(a); //lint 30 

}