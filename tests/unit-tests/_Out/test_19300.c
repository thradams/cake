
#pragma safety enable


struct X {
    int a;
    union {
        char b;
        long c;

        union {
            bool d;
            short e;
        };

        double f;
    };
    struct Y
    {
        int g;
    } y;
};

void f()
{
    struct X s = { 0 };
    s.f = 1; 
}
