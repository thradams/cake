// Test 3: _Atomic qualifier on unsupported type

_Atomic int unsupported_atomic_var = 0; // Error: Atomic types are not supported by the implementation

int main() {
    return 0;
}
