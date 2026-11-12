/*
   A bit-field with an enum type, initialized and assigned. With
   -no-bitfields its initializer is a store in the tiles, like the one of an
   integer bit-field (the values used fit in the field with any signedness).
*/

#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

enum kind
{
    KIND_NONE,
    KIND_ONE,
    KIND_TWO,
    KIND_THREE
};

struct item
{
    enum kind kind : 4;
    unsigned count : 6;
    enum kind other : 4;
};

static struct item file_item = { KIND_TWO, 33, KIND_ONE };

static enum kind get(enum kind k)
{
    return k;
}

int main(void)
{
    struct item a = { KIND_THREE, 63, KIND_ONE };
    struct item b = { .other = KIND_TWO };
    struct item c = { get(KIND_ONE), 5, get(KIND_THREE) };
    struct item d[2] = { { KIND_ONE }, { .count = 7, .kind = get(KIND_TWO) } };

    CHECK(file_item.kind == KIND_TWO && file_item.count == 33 && file_item.other == KIND_ONE);
    CHECK(a.kind == KIND_THREE && a.count == 63 && a.other == KIND_ONE);
    CHECK(b.kind == KIND_NONE && b.count == 0 && b.other == KIND_TWO);
    CHECK(c.kind == KIND_ONE && c.count == 5 && c.other == KIND_THREE);
    CHECK(d[0].kind == KIND_ONE && d[0].count == 0 && d[1].kind == KIND_TWO && d[1].count == 7);

    a.kind = KIND_ONE;
    a.other = get(KIND_THREE);
    CHECK(a.kind == KIND_ONE && a.count == 63 && a.other == KIND_THREE);

    return failures;
}
