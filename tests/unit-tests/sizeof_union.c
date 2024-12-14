#pragma safety enable



union X {
    struct {int a, b; } y;
    double d;
};
static_assert( sizeof (union X) == 8);