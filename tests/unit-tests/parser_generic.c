void f(double) {}
int main(void)
{
    //error: duplicate default generic association.
    [[cake::e1570]]
    _Generic((1.0), float: f, default: f, default: f)(1.0);
}
