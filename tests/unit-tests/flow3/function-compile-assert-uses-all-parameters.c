/* the conditions after the declarator see every parameter */
void f(int a, int b) compile_assert(a > b);

void g(int x, int y)
{
    f(2, 1);
    f(1, 2); //lint 67
    f(x, y); //lint 67
    if (x > y)
        f(x, y);
}
