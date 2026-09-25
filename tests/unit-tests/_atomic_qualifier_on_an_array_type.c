// Test 4: _Atomic qualifier on an array type

_Atomic int atomic_array[5]; /* array of atomic int: ok */

typedef int A[5];
_Atomic A x; //lint 2090 _Atomic qualifier cannot be applied to array types

_Atomic(int[5]) y; //lint 2090

int main() {
    return 0;
}
