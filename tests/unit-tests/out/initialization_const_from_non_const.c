struct X {
    int x;
};


int main()
{
    struct X  x;
    struct X  __cmp_lt_0;

    __cmp_lt_0.x = 50;
    x = __cmp_lt_0;
}

