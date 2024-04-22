#define _is_same(T1, T2) _Generic(T1, T2: 1, default: 0)
typedef char *T1;
typedef T1 (*f[3])(int);

