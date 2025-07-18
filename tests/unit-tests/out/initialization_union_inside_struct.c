union U {
    int b;
    double c;
};

struct X {
    double a;
    union U  u;
    int d;
};


int main()
{
    struct X  x;

    x.a = 1;
    x.u.b = 2;
    x.d = 3;
}

