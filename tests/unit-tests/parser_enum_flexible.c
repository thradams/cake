enum A
{
    A = 11111111111L
};

enum B
{
    B = 11111111111L,
    C = -1
};


#ifdef _WIN32
static_assert(_Generic(enum A, unsigned long long: 1, default: 0));
static_assert(_Generic(enum B, signed  long long: 1, default: 0));
#else
static_assert(_Generic(enum A, unsigned long: 1, default: 0));
static_assert(_Generic(enum B, signed long: 1, default: 0));
#endif
