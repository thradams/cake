// Test 9: constexpr with non-constant expression initializer

int x = 10;
constexpr int d = x; //lint 1860 requires a compile time object

int main() {
    return 0;
}
