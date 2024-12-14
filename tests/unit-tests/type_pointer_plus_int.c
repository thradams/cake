#define _is_same(T1, T2) _Generic(T1, T2: 1, default: 0)
int *p = 0;
static_assert(_is_same(typeof(*(p + 1)), int));
