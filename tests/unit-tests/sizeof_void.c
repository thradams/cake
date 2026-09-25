int f(void)
{
    return (int)sizeof(void); //lint 860 invalid application of 'sizeof' to void type
}
