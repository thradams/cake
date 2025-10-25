// Test 7: _Atomic qualifier on multi-dimensional array type

_Atomic int atomic_matrix[3][3]; // Error: _Atomic qualifier cannot be applied to array types

int main() {
    return 0;
}
