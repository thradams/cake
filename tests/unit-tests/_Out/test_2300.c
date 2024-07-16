#define _is_same(T1, T2) _Generic(T1, T2: 1, default: 0)
typedef int A[2];
typedef A *B[1];

