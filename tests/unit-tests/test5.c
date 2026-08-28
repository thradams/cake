struct X
{
   int a;
   int b;
   int c;
};

struct Y
{
   struct X x;
   struct X x2;
   int d;
};

int main()
{
    constexpr struct Y y = { 1, 2, 3, 4, 5,6 , 7};
    static_assert(y.x.a == 1);
    static_assert(y.x.b == 2);
    static_assert(y.x.c == 3);

    static_assert(y.x2.a == 4);
    static_assert(y.x2.b == 5);
    static_assert(y.x2.c == 6);

    static_assert(y.d == 7);
}