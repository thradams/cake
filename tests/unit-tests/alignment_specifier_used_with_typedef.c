// Test 1: Alignment specifier used with typedef

typedef alignas(16) int aligned_typedef; //lint 2150 alignment specifier cannot be used with 'typedef'

int main() {
    return 0;
}
