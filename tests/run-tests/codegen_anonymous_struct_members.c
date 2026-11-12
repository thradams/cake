/*
   An anonymous struct member is named __mN in the generated code, N being its
   position among the anonymous members. Access, store and initialization have
   to use the same name as the declaration.
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

struct S
{
    struct { int a; };
    struct { int b; };
    int c;
    union { int d; float e; };
};

struct Deep
{
    struct { int x; };
    struct
    {
        int y;
        struct { int z; };
        struct { int w; };
    };
};

int main(void)
{
    struct S s = { 0 };
    s.a = 1;
    s.b = 2;
    s.c = 3;
    s.d = 4;
    CHECK(s.a == 1);
    CHECK(s.b == 2);
    CHECK(s.c == 3);
    CHECK(s.d == 4);

    struct S* p = &s;
    p->b = 20;
    CHECK(s.b == 20);
    CHECK(s.a == 1);
    CHECK(p->b + p->a == 21);

    struct S t = { { 10 }, { 20 }, 30, { 40 } };
    CHECK(t.a == 10);
    CHECK(t.b == 20);
    CHECK(t.c == 30);
    CHECK(t.d == 40);

    struct Deep d = { 0 };
    d.x = 1;
    d.y = 2;
    d.z = 3;
    d.w = 4;
    CHECK(d.x == 1);
    CHECK(d.y == 2);
    CHECK(d.z == 3);
    CHECK(d.w == 4);

    return failures;
}
