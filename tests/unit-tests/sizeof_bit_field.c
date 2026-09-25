struct s { int b : 3; };
int f(struct s x)
{
    return (int)sizeof(x.b); //lint 860 invalid application of 'sizeof' to a bit-field
}
