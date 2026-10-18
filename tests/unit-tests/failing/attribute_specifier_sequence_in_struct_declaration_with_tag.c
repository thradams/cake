// Test 5: Attribute specifier sequence in struct declaration with tag

struct __attribute__((aligned(16))) MyStruct { // Error: Attribute specifier not allowed with struct tag
    int x;
    int y;
};

int main() {
    return 0;
}
