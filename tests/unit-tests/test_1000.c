#define _is_same(T1, T2) _Generic(T1, T2 : 1, default: 0)
int i; 
int (*f)(void);
static_assert(_is_same(typeof(&i), int *));
static_assert(_is_same(typeof(&f), int (**)(void)));
