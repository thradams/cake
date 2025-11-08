


void T00()
{
    // warning: warning: excess elements in initializer
    [[cake::w62]]
    constexpr int i[2] = {1, 2, 3};

    // error: array index '3' in initializer exceeds array bounds
    [[cake::e720]]
    constexpr int i2[2] = {1,[3] = 2};

    //error: array designator value '-1' is negative
    [[cake::e720]]
    constexpr int i3[2] = {1,[2 - 3] = 2};

    constexpr int i5 = {{1}}; //ok

    //warning: warning: excess elements in initializer
    [[cake::w62]]
    constexpr int i4 = {1, 2};
}

void T0()
{
    constexpr int i3 = {{3}};
    static_assert(i3 == 3);

    constexpr int i = 1;
    static_assert(i == 1);

    constexpr int i2 = {2};
    static_assert(i2 == 2);
}

void T1()
{
    struct X
    {
        int a;
        int b;
    };

    struct Y
    {
        int i;
        struct X x;
        struct X x2;
    };

    constexpr struct Y y = {1, {1, 2}, 3};
    static_assert(y.i == 1);

    static_assert(y.x.a == 1);
    static_assert(y.x.b == 2);

    static_assert(y.x2.a == 3);
}


void T2()
{
    struct Y {
        int a;
        int ar[3];
        int b;
    };

    constexpr struct Y y = {.ar[1] = 2, 3, 4};

    static_assert(y.a == 0);
    static_assert(y.ar[0] == 0);
    static_assert(y.ar[1] == 2);
    static_assert(y.ar[2] == 3);
    static_assert(y.b == 4);
}


void T3()
{
    struct X
    {
        int i;
    };
    struct Y
    {
        int i;
        struct X x;
    };
    struct X x = {0};
    struct Y y = {.x = x, .i = 4};
}

void T4()
{
    //en.cppreference.com/w/c/language/array_initialization.html
    // The following four array declarations are the same

        //{1, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 4, 5, 6};
    constexpr short q4[4][3][2] = {1,[1] = 2, 3,[2] = 4, 5, 6};
    static_assert(q4[0][0][0] == 1);

    static_assert(q4[1][0][0] == 2);
    static_assert(q4[1][0][1] == 3);

    static_assert(q4[2][0][0] == 4);
    static_assert(q4[2][0][1] == 5);
    static_assert(q4[2][1][0] == 6);

    constexpr short q1[4][3][2] = {{1}, {2, 3}, {4, 5, 6}};

    static_assert(q1[0][0][0] == 1);

    static_assert(q1[1][0][0] == 2);
    static_assert(q1[1][0][1] == 3);

    static_assert(q1[2][0][0] == 4);
    static_assert(q1[2][0][1] == 5);
    static_assert(q1[2][1][0] == 6);

    constexpr short q2[4][3][2] = {1, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 4, 5, 6};

    static_assert(q2[0][0][0] == 1);

    static_assert(q2[1][0][0] == 2);
    static_assert(q2[1][0][1] == 3);

    static_assert(q2[2][0][0] == 4);
    static_assert(q2[2][0][1] == 5);
    static_assert(q2[2][1][0] == 6);


    constexpr short q3[4][3][2] = {{
                                       {1},
                                   },
                                   {
                                       {2, 3},
                                   },
                                   {
                                       {4, 5},
                                       {6},
                                   }};


    static_assert(q3[0][0][0] == 1);

    static_assert(q3[1][0][0] == 2);
    static_assert(q3[1][0][1] == 3);

    static_assert(q3[2][0][0] == 4);
    static_assert(q3[2][0][1] == 5);
    static_assert(q3[2][1][0] == 6);
}

void T5()
{
    constexpr int a[2][3] = {1, 2, 3, 4, 5, 6};

    static_assert(a[0][0] == 1);
    static_assert(a[0][1] == 2);
    static_assert(a[0][2] == 3);
    static_assert(a[1][0] == 4);
    static_assert(a[1][1] == 5);
    static_assert(a[1][2] == 6);
}

void T6()
{
    constexpr char s[] = "123";
    constexpr char s2[] = s;
    static_assert(s2[0] == '1');
}

void T7()
{
    // initializes w (an array of two structs) to
    // { { {1,0,0}, 0}, { {2,0,0}, 0} }
    constexpr struct { int a[3], b; } w[] = {[0] .a = {1},[1].a[0] = 2};
    static_assert(_Countof(w) == 2);
    static_assert(w[0].a[0] == 1);
    static_assert(w[0].a[1] == 0);
    static_assert(w[0].a[2] == 0);
    static_assert(w[0].b == 0);
}


void T8()
{

    constexpr struct data {
        int nr;
        char const* value;
    } dat[] = {
        {1, "Foo"}, {2, "Bar"}, {3, "Hello"}, {4, "World"}
    };

    static_assert(dat[0].nr == 1);
    static_assert(dat[1].nr == 2);
    static_assert(dat[2].nr == 3);
    static_assert(dat[3].nr == 4);
}


void T9()
{
    constexpr int x[] = {1, 3, 5}, * p = x;
    static_assert(x[0] == 1);
    static_assert(x[1] == 3);
    static_assert(x[2] == 5);

    constexpr int a[] = {1, 2, 3};
    static_assert(a[0] == 1);
    static_assert(a[1] == 2);
    static_assert(a[2] == 3);
    static_assert(_Countof(a) == 3);
}


void T10()
{
    constexpr int a[3] = {[2] = 1,[1] = 2,[0] = 3};
    static_assert(a[0] == 3);
    static_assert(a[1] == 2);
    static_assert(a[2] == 1);
}


void T11()
{
    struct X {
        int a, b, c;
    };

    constexpr struct X x = {.b = 2, 3};
    static_assert(x.a == 0);
    static_assert(x.b == 2);
    static_assert(x.c == 3);
}


void T12()
{
    struct X {
        int a, b, c;
    };
    //warning: warning: excess elements in initializer
    [[cake::w62]]
    constexpr struct X x = {.c = 3, 4};

    static_assert(x.a == 0);
    static_assert(x.b == 0);
    static_assert(x.c == 3);
}



void T13()
{
    struct X {
        int a, b, c;
    };

    //error: member 'd' not found in 'X'
    [[cake::e720]]
    constexpr struct X x = {.d = 3};

    static_assert(x.a == 0);
    static_assert(x.b == 0);
    static_assert(x.c == 0);
}



