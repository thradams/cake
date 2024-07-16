#define _is_same(T1, T2) _Generic(typeof(T1), typeof(T2) : 1, default: 0)
int i; 
int (*f)(void);


