struct point { int x; };
int f(struct point p)
{
    return 1 || p; //lint 880 right operand of '||' must have scalar type
}
