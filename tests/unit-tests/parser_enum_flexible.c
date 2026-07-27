enum A
{
    A = 11111111111L
};

static_assert(_Generic(enum A, unsigned long: 1, default: 0));

enum B
{
    B = 11111111111L,
    C = -1
};

static_assert(_Generic(enum B, signed long: 1, default: 0));
