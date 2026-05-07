struct X { int i; };
struct X f()
{
    struct X x = {};
    return x;
}

int main()
{
    //error: lvalue required as left operand of assignment
    f().i = 1; //lint 1230
}


