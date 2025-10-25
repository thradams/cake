// Test 6: constexpr with atomic type


constexpr _Atomic int a = 10; // Error: constexpr cannot have atomic type

int main() {
    return 0;
}
