#pragma safety enable

/* a loop whose condition never holds: its body is unreachable code (warning 68) */

void pointer_null(void)
{
    int* _Opt e = 0;
    while (e)
    {
        *e = 1; //lint 68
    }
}

void value_zero(void)
{
    int x = 0;
    while (x)
    {
        x = 2; //lint 68
    }
}

void body_breaks(void)
{
    int* _Opt e = 0;
    while (e)
    {
        *e = 1; //lint 68
        break;
    }
}
