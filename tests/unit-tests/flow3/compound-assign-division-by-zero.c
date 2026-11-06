/* a /= b is a / b: every value of b is checked */
#pragma safety enable

void f(int a)
{
    int b = 0;
    a /= b; //lint 36
}
