int f(int x)
{
    return *x; //lint 780 indirection requires pointer operand (and no 'indirection in array' warning)
}
