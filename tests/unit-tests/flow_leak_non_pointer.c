#pragma safety enable

struct X {
    int _Owner i;
};

void f()
{
    const struct X x [[cake::leak]] = {0};
}



