#define _is_same(T1, T2) _Generic(T1, T2 : 1, default: 0)
int a[10];
 static_assert(_is_same(typeof(&a) ,int (*)[10]));
