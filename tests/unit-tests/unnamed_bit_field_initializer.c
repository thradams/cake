/*
  An unnamed bit-field takes no initializer (6.7.11): the value goes to
  the next named member.
*/
struct S
{
    char a : 7;
    char b : 3;
    int : 0;
    int c : 2;
    long long g : 40;
    int : 3;
    int h : 30;
};

constexpr struct S s = { -50, -2, 1, 5, 6 };
static_assert(s.a == -50);
static_assert(s.b == -2);
static_assert(s.c == 1);
static_assert(s.g == 5);
static_assert(s.h == 6);
