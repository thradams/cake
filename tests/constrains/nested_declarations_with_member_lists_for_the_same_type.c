// Test 2: Nested declarations with member lists for the same type

struct NestedStruct {
    int x;
    struct NestedStruct { // Error: Cannot nest declarations with member lists
        int y;
    };
};

int main() {
    return 0;
}
