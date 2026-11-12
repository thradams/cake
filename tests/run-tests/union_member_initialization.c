/*
   Initialization of a block-scope union: the member that has the
   initializer is the one initialized, also when it is a struct (named or
   anonymous) or is not the first member, and the other members are not
   assigned.
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

union anonymous_first
{
    struct
    {
        unsigned char lo;
        unsigned char hi;
    };
    unsigned short word;
};

union anonymous_second
{
    unsigned short word;
    struct
    {
        unsigned char lo;
        unsigned char hi;
    };
};

union named_first
{
    struct
    {
        unsigned char lo;
        unsigned char hi;
    } bytes;
    unsigned short word;
};

union small_first
{
    char c;
    int i;
};

struct with_union
{
    union
    {
        struct
        {
            int a;
            int b;
        };
        long long ll;
    };
    int tail;
};

static int id(int x)
{
    return x;
}

/* fills the stack with a value that is not zero */
static void dirty(void)
{
    volatile char junk[256];
    for (int i = 0; i < 256; i++)
        junk[i] = 0x7f;
}

static void positional(void)
{
    union anonymous_first a = { 1, 2 };
    union anonymous_first b = { id(1), id(2) };
    union named_first c = { { 3, id(4) } };

    CHECK(a.lo == 1 && a.hi == 2);
    CHECK(b.lo == 1 && b.hi == 2);
    CHECK(c.bytes.lo == 3 && c.bytes.hi == 4);
}

static void designated(void)
{
    union anonymous_first a = { .word = 0x0102 };
    union anonymous_first b = { .word = id(0x0102) };
    union anonymous_first c = { .hi = id(7) };
    union anonymous_second d = { .lo = 1, .hi = 2 };
    union anonymous_second e = { .hi = id(2) };
    union named_first f = { .word = id(0x0304) };
    union named_first g = { .bytes.hi = 5 };

    CHECK(a.word == 0x0102);
    CHECK(b.word == 0x0102);
    CHECK(c.hi == 7 && c.lo == 0);
    CHECK(d.lo == 1 && d.hi == 2);
    CHECK(e.hi == 2 && e.lo == 0);
    CHECK(f.word == 0x0304);
    CHECK(g.bytes.hi == 5 && g.bytes.lo == 0);
}

static void inside_struct(void)
{
    struct with_union a = { .ll = id(5), .tail = 2 };
    struct with_union b = { .b = id(5) };
    struct with_union c = { .a = 1, .b = 2, .tail = id(3) };

    CHECK(a.ll == 5 && a.tail == 2);
    CHECK(b.a == 0 && b.b == 5 && b.tail == 0);
    CHECK(c.a == 1 && c.b == 2 && c.tail == 3);
}

static void smaller_member(void)
{
    /* the member does not cover the union: the other bytes are zero */
    union small_first a = { .c = id(1) };
    union small_first b = { 0 };
    union small_first c = { .i = id(9) };

    CHECK(a.c == 1 && a.i == 1);
    CHECK(b.i == 0);
    CHECK(c.i == 9);
}

static void compound_literal(void)
{
    union anonymous_first* p = &(union anonymous_first){ id(1), 2 };
    union anonymous_second* q = &(union anonymous_second){ .hi = id(3) };

    CHECK(p->lo == 1 && p->hi == 2);
    CHECK(q->hi == 3 && q->lo == 0);
}

int main(void)
{
    dirty();
    positional();
    dirty();
    designated();
    dirty();
    inside_struct();
    dirty();
    smaller_member();
    dirty();
    compound_literal();

    return failures;
}
