// Test 4: _Atomic qualifier on an array type

_Atomic int atomic_array[5]; // Error: _Atomic qualifier cannot be applied to array types

int main() {
    return 0;
}
