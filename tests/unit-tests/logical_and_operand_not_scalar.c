struct point { int x; };
int f(struct point p)
{
    return p && 1; //lint 870 left operand of '&&' must have scalar type
}
