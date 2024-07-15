

typedef char *A;
typedef const A *B;
static_assert(_Generic(typeof(B), char *const *: 1));

typedef const int T;
T i = 0;
static_assert(_Generic(typeof(i), const int: 1));

const T i2=0;
static_assert(_Generic(typeof(i2), const int: 1));

typedef int T3;
const T3 i3=0;
static_assert(_Generic(typeof(i3), const int: 1));
