struct Y {
    double d;
};

struct X {
    struct Y  y;
    int i;
};


void T2()
{
    struct Y  y;
    struct X  x;

    y.d = 3.000000;
    x.y.d = 0;
    x.i = 2;
}

