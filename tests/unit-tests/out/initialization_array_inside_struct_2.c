struct X {
    int a;
    int b;
};

struct Y {
    int i;
    struct X  x;
    struct X  x2;
};


void T1()
{
    struct Y  y;

    y.i = 1;
    y.x.a = 1;
    y.x.b = 2;
    y.x2.a = 3;
    y.x2.b = 0;
}

