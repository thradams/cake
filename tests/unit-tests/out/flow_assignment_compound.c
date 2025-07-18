struct X {
    int i;
    void * p;
};


int main()
{
    struct X  x;
    struct X  __cmp_lt_0;

    __cmp_lt_0.i = 0;
    __cmp_lt_0.p = 0;
    x = __cmp_lt_0;
}

