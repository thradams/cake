/*
   C23 6.2.5/6.3.1.1/6.4.4.1: the type of _BitInt expressions and literals.
   _BitInt does not promote to int (unary +, ~, and binary operators between
   two _BitInt keep the width), wb/uwb literals have the smallest width that
   holds the value, and _BitInt can be a bit-field type.
*/
#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

typedef _BitInt(7) i7;

struct S
{
    _BitInt(12) bf : 5;
    unsigned _BitInt(12) ubf : 3;
};

static _BitInt(24) add24(_BitInt(24) a, _BitInt(24) b) { return a + b; }

static_assert(_Generic(5wb, _BitInt(4): 1, default: 0));
static_assert(_Generic(255uwb, unsigned _BitInt(8): 1, default: 0));
static_assert(_Generic(-1wb, _BitInt(2): 1, default: 0));
static_assert(_Generic(0wb, _BitInt(2): 1, default: 0));
static_assert(_Generic(0uwb, unsigned _BitInt(1): 1, default: 0));
static_assert(sizeof(0x7FFFFFFFFFFFFFFFwb) >= 8);
static_assert((_BitInt(6))15 + (_BitInt(6))1 == 16);
static_assert(_Generic((_BitInt(8))0, _BitInt(8): 1, default: 0));
static_assert(!_Generic((_BitInt(8))0, _BitInt(9): 1, default: 0));
static_assert(-7 / (_BitInt(8))2 == -3);
static_assert(-7 % (_BitInt(8))2 == -1);
static_assert(((_BitInt(8))-1) < ((_BitInt(8))0));
static_assert(((unsigned _BitInt(8))255) > ((unsigned _BitInt(8))0));

int main(void)
{
    i7 a = 62, b = 1;

    static_assert(_Generic(a + b, _BitInt(7): 1, default: 0));
    static_assert(_Generic(a + 1, int: 1, default: 0));      /* int is wider: computed in int */
    static_assert(_Generic(+a, _BitInt(7): 1, default: 0));  /* no integer promotion */
    static_assert(_Generic(~a, _BitInt(7): 1, default: 0));
    CHECK(a + b == 63 && a + 1 == 63);

    struct S st = { -5, 7 };
    CHECK(st.bf == -5);
    CHECK(st.ubf == 7);
    st.ubf += 1;
    CHECK(st.ubf == 0);

    _BitInt(24) big = 8388607;
    CHECK(add24(big, 1) == -8388608);

    _BitInt(16) arr[4] = { 1, 2, 3, 4 };
    static_assert(sizeof arr == 4 * sizeof(_BitInt(16)));
    CHECK(arr[3] == 4);

    enum { K = (unsigned _BitInt(4))15 };
    static_assert(K == 15);

    return failures;
}
