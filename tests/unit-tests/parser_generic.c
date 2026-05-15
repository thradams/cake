void f(double) {}
int main(void)
{
    //error: duplicate default generic association.

    _Generic((1.0), float: f, default: f, default: f)(1.0); //lint 1570

    //expressions are evaluated
    static_assert(_Generic(0, double: 0.0, int: -2) == -2);

    int i;
    _Generic(i, int:1, int:2); //lint 1570 two compatible types.
}

