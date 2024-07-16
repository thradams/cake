
#define _is_same_type(T1, T2) _Generic(typeof(T1), typeof(T2): 1, default: 0)

char((a1));
char b1;
static_assert(_is_same_type(a1, b1));

char((a2))[2];
char b2[2];
static_assert(_is_same_type(a2, b2));

char((a3))(int(a));
char(b3)(int a);
static_assert(_is_same_type(a3, b3));
