struct X { int i; };
struct X f()
{
    struct X x = {};
    return x;
}

int main()
{
    //error: lvalue required as left operand of assignment
    [[cake::e1230]]
    f().i = 1;
}


