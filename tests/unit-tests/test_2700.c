#define _is_same(T1, T2) _Generic(T1, T2: 1, default: 0)
static_assert(_is_same(typeof("A"), char[2]));
static_assert(_is_same(typeof("AB"), char[3]));
