
int f(int a[2])
{
    int i;
    //warning: sizeof applied to array function parameter 
    [[cake::w47]]
    i = sizeof(a);

    //warning: sizeof applied to array function parameter
    [[cake::w47]]
    static_assert(sizeof(a) == sizeof(void*));
}

int main()
{
    int i;
    //error: struct is incomplete type
    [[cake::e740]]
    i = sizeof(struct X);
}

