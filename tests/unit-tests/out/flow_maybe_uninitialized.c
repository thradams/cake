struct X {
    int i;
};


void f(struct X * x, int i)
{
    if (i > 0)
    {
        x->i = 1;
    }
}

int printf(char * format, ...);

int main()
{
    struct X  x;

    f(&x, 1);
    printf("%d", x.i);
}

