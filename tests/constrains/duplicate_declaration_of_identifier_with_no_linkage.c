// Test 2: Duplicate declaration of identifier with no linkage

int main() {
    int x = 5;
    int x = 10; // Error: Duplicate declaration of identifier with no linkage in the same scope
    return 0;
}
