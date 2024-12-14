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
    struct X  x = {1, 2, 3};
}

