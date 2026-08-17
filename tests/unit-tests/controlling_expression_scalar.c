struct X { int i; };

int main()
{
    struct X x = { 0 };

    if (x) {} //lint 940 controlling expression must have scalar type

    for (; x;) {} //lint 940 controlling expression must have scalar type

    return 0;
}
