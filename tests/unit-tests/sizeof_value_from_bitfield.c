/*
   sizeof cannot be applied to a bit-field member, but a value computed from
   one is not a bit-field: its type is the promoted type.
*/

struct S
{
    unsigned a : 3;
    unsigned long long big : 40;
    int c : 4;
};

struct S s;

static_assert(sizeof(s.a + 0) == sizeof(int));
static_assert(sizeof(s.big + 0) == sizeof(long long));
static_assert(sizeof(+s.a) == sizeof(int));
static_assert(sizeof(-s.c) == sizeof(int));
static_assert(sizeof(~s.big) == sizeof(long long));
static_assert(sizeof(s.c * 2) == sizeof(int));
static_assert(sizeof(s.a & 1) == sizeof(int));
static_assert(sizeof(s.big << 1) == sizeof(long long));
static_assert(sizeof((int)s.a) == sizeof(int));
static_assert(sizeof((s.a, s.a)) == sizeof(int));

int f(void)
{
    return sizeof(s.a); //lint 860 error: invalid application of 'sizeof' to a bit-field
}
