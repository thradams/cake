void g(void);
int f(void)
{
    return (int)sizeof(g); //lint 53 sizeof applied to a function type
}
