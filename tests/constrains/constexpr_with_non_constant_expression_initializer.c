// Test 9: constexpr with non-constant expression initializer

int x = 10;
constexpr int d = x; // Error: constexpr initializer must be a constant expression

int main() {
    return 0;
}
