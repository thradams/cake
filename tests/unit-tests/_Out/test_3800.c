
#define _is_same_type(T1, T2) _Generic(typeof(T1), typeof(T2): 1, default: 0)

char((a1));
char b1;


char((a2))[2];
char b2[2];


char((a3))(int(a));
char(b3)(int a);

