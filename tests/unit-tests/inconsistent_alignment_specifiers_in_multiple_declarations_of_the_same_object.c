// Test 8: Inconsistent alignment specifiers in multiple declarations of the same object

alignas(16) int var;

int var; //lint 2310

int main() {
    return 0;
}
