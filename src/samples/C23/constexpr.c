#include <stdio.h>

constexpr int c = 123;

constexpr int c2 = c + 1000;

int a[c];

constexpr double PI = 3.14;


static_assert(PI + 1 == 3.14 + 1.0);

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
    static_assert(y.ar[1] + y.ar[2] == 5);
}

static_assert("abc"[0] == 'a');


int main()
{
    constexpr char ch = 'a';

    printf("%f %c", PI, ch);
}
