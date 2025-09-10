void f(bool b);

int main(void)
{
    //warning: implicit conversion of nullptr constant to 'bool'
    [[cake::w49]]
    bool b = nullptr;

    //warning: implicit conversion of nullptr constant to 'bool'
    [[cake::w49]]
    b = nullptr;

    //warning: implicit conversion of nullptr constant to 'bool'
    [[cake::w49]]
    f(nullptr);

}
