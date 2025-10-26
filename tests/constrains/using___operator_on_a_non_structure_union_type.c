// Test 1: Using . operator on a non-structure/union type

int main() {
    int x = 10;
    x.member = 5; // Error: x is not a structure or union
    return 0;
}
