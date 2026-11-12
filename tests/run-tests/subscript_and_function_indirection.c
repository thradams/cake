/*
   - E1[E2] is *((E1)+(E2)): the pointer can be the operand inside the
     brackets, 1[p] is p[1];
   - a function designator is converted to a pointer to the function, so *
     can be applied to it any number of times: (***f)(x).
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

struct point
{
    int x;
    int y;
};

static int twice(int x)
{
    return x * 2;
}

static int calls = 0;

static int next(void)
{
    return calls++;
}

int main(void)
{
    int a[4] = { 10, 20, 30, 40 };
    int* p = a;
    struct point points[2] = { { 1, 2 }, { 3, 4 } };
    struct point* q = points;
    int i = 2;
    const char* text = "abc";

    CHECK(1[p] == 20);
    CHECK(i[a] == 30);
    CHECK((i + 1)[a] == 40);
    CHECK(1[q].y == 4);
    CHECK(0[points].x == 1);
    CHECK(2[text] == 'c');

    3[a] = 7;
    CHECK(a[3] == 7);

    i[p]++;
    CHECK(a[2] == 31);

    CHECK(next()[a] == 10);
    CHECK(calls == 1);

    int (*fp)(int) = twice;

    CHECK((*twice)(1) == 2);
    CHECK((**twice)(2) == 4);
    CHECK((*fp)(3) == 6);
    CHECK((**fp)(4) == 8);
    CHECK((***fp)(5) == 10);
    CHECK((*&twice)(6) == 12);

    return failures;
}
