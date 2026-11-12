/*
   A struct that is a member or an element of the object being initialized
   can be initialized by any expression of its type, not only by the name of
   another object: a function call, a member of another object, *p, a[i],
   a conditional, a compound literal.
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

struct point
{
    int x;
    int y;
};

struct line
{
    int id;
    struct point from;
    struct point to;
};

union value
{
    struct point p;
    long long ll;
};

static struct point origin = { 5, 6 };

static struct point make(int x, int y)
{
    struct point p = { x, y };
    return p;
}

static int id(int x)
{
    return x;
}

int main(void)
{
    struct point a = { 1, 2 };
    struct point* p = &a;
    struct line l = { 9, { 10, 11 }, { 12, 13 } };
    struct point points[2] = { { 20, 21 }, { 22, 23 } };

    struct point from_call[2] = { make(1, 2), make(3, 4) };
    struct point from_member[2] = { l.from, l.to };
    struct point from_pointer[1] = { *p };
    struct point from_element[1] = { points[id(1)] };
    struct point from_conditional[2] = { id(1) ? a : origin, id(0) ? a : origin };
    struct point from_literal[1] = { (struct point){ 7, 8 } };
    struct point from_name[1] = { a };

    CHECK(from_call[0].x == 1 && from_call[0].y == 2 && from_call[1].x == 3 && from_call[1].y == 4);
    CHECK(from_member[0].x == 10 && from_member[0].y == 11 && from_member[1].x == 12 && from_member[1].y == 13);
    CHECK(from_pointer[0].x == 1 && from_pointer[0].y == 2);
    CHECK(from_element[0].x == 22 && from_element[0].y == 23);
    CHECK(from_conditional[0].x == 1 && from_conditional[1].x == 5);
    CHECK(from_literal[0].x == 7 && from_literal[0].y == 8);
    CHECK(from_name[0].x == 1 && from_name[0].y == 2);

    struct line l1 = { 1, make(1, 2), l.to };
    struct line l2 = { .to = l.from, .to.y = 99 };
    struct line l3 = { .id = id(3), .from = *p };
    struct line lines[2] = { { 1, make(1, 2) }, { .to = origin } };
    union value v = { make(4, 5) };

    CHECK(l1.id == 1 && l1.from.x == 1 && l1.from.y == 2 && l1.to.x == 12 && l1.to.y == 13);
    CHECK(l2.id == 0 && l2.from.x == 0 && l2.to.x == 10 && l2.to.y == 99);
    CHECK(l3.id == 3 && l3.from.x == 1 && l3.from.y == 2 && l3.to.x == 0);
    CHECK(lines[0].from.y == 2 && lines[0].to.x == 0 && lines[1].id == 0 && lines[1].to.y == 6);
    CHECK(v.p.x == 4 && v.p.y == 5);

    return failures;
}
