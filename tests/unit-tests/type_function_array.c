#define _is_same(T1, T2) _Generic(T1, T2 : 1, default: 0)
void (*pf[10])(void* val);
static_assert(_is_same(typeof(pf[0]), void (*)(void* val)));
