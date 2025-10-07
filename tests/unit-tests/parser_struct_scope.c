struct X {
    char i;
};

int main()
{
    struct X { double d; } x;
    x.d = 1.2;
    static_assert(sizeof(x.d) == sizeof(double));
}

void f()
{
    struct X x;
    static_assert(sizeof(x.i) == sizeof(char));
}
