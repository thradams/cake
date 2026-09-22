// Test 2: Duplicate declaration of identifier with no linkage

int main() {
    int x = 5;
    int x = 10; //lint 1020 redeclaration
    return 0;
}
