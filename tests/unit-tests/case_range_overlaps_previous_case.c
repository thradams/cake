int f(int x)
{
    switch (x)
    {
        case 1: return 1;
        case 0 ... 5: return 2; //lint 1450 case range '0' ... '5' overlaps a previous case
    }
    return 0;
}
