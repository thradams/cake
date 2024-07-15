enum E : long long { R, G, B } e;
static_assert (_Generic (e, long long: 1, default: 0) == 1, "E type");

