
void T10()
{
    constexpr char s[] = "123";
    constexpr char s2[] = s;
    static_assert(s2[0] == '1');
}

void T9()
{
    // initializes w (an array of two structs) to
    // { { {1,0,0}, 0}, { {2,0,0}, 0} }
    constexpr struct { int a[3], b; } w[] = { [0] .a = {1},[1].a[0] = 2 };
    static_assert(_Lengthof(w) == 2);
    static_assert(w[0].a[0] == 1);
    static_assert(w[0].a[1] == 0);
    static_assert(w[0].a[2] == 0);
    static_assert(w[0].b == 0);
}

int T1(void)
{
    //en.cppreference.com/w/c/language/array_initialization.html
    // The following four array declarations are the same
    //short q1[4][3][2] = {
    //    { 1 },
    //    { 2, 3 },
    //    { 4, 5, 6 }
    //};

    //short q2[4][3][2] = {1, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 4, 5, 6};

    constexpr short q3[4][3][2] = {
        {
            { 1 },
        },
        {
            { 2, 3 },
        },
        {
            { 4, 5 },
            { 6 },
        }
    };

    static_assert(q3[0][0][0] == 1);
    //{1, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 4, 5, 6};
    //short q4[4][3][2] = {1, [1]=2, 3, [2]=4, 5, 6};


    // Character names can be associated with enumeration constants
    // using arrays with designators:
  //  enum { RED, GREEN, BLUE };
   // const char *nm[] = {
   //     [RED] = "red",
  //      [GREEN] = "green",
  //      [BLUE] = "blue",
//};
}



void T2()
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

struct Y {
    int a;
    int ar[3];
    int b;
};

void T3()
{
    constexpr struct Y y = { .ar[1] = 2, 3, 4 };
    static_assert(y.a == 0);
    static_assert(y.ar[0] == 0);
    static_assert(y.ar[1] == 2);
    static_assert(y.ar[2] == 3);
    static_assert(y.b == 4);
}

void T4()
{
    constexpr int x[] = { 1, 3, 5 }, * p = x;
    static_assert(x[0] == 1);
    static_assert(x[1] == 3);
    static_assert(x[2] == 5);

    constexpr int a[] = { 1, 2, 3 };
    static_assert(a[0] == 1);
    static_assert(a[1] == 2);
    static_assert(a[2] == 3);
    static_assert(_Lengthof(a) == 3);
}


void T5()
{
    constexpr int a[3] = { [2] = 1,[1] = 2,[0] = 3 };
    static_assert(a[0] == 3);
    static_assert(a[1] == 2);
    static_assert(a[2] == 1);
}


struct X {
    int a, b, c;
};

void T6()
{
    constexpr struct X x = { .b = 2, 3 };
    static_assert(x.a == 0);
    static_assert(x.b == 2);
    static_assert(x.c == 3);
}


void T7()
{
    constexpr struct X x = { .c = 3, 4 };
    static_assert(x.a == 0);
    static_assert(x.b == 0);
    static_assert(x.c == 3);
}




void T8()
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


    int main()
    {
        struct Y y = { 1, {1, 2}, 3 };
    }
}
