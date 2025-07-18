
unsigned char  init(int * a)
{
    *a = 3;
    return 1;
}

int main()
{
    int a;

    if (init(&a) || a == 0)
    {
        return a;
    }
    if (init(&a) && a == 0)
    {
        return a;
    }
    return a;
}

