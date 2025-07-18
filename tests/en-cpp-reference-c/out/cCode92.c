
int fermat()
{
    int MAX;

    MAX = 1000;
    {
        int a;
        int b;
        int c;
        a = 1;
        b = 1;
        c = 1;
        for (; 1; )
        {
            if (((a * a * a) == ((b * b * b) + (c * c * c))))
            {
                return 1;
            }
            ++a;
            if (a > 1000)
            {
                a = 1;
                ++b;
            }
            if (b > 1000)
            {
                b = 1;
                ++c;
            }
            if (c > 1000)
            {
                c = 1;
            }
        }
    }
    return 0;
}

int __cdecl puts(char * _Buffer);

int main(void)
{
    if (fermat())
    {
        puts("Fermat's Last Theorem has been disproved.");
    }
    else
    {
        puts("Fermat's Last Theorem has not been disproved.");
    }
}

