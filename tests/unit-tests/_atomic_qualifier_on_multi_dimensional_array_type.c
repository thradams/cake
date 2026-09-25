// Test 7: _Atomic qualifier on multi-dimensional array type

_Atomic int atomic_matrix[3][3]; /* array of atomic int: ok */

typedef int M[3][3];
_Atomic M x; //lint 2090 _Atomic qualifier cannot be applied to array types

int main() {
    return 0;
}
