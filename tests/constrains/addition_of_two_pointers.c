// Test 1: Addition of two pointers

int main() {
    int a = 10, b = 20;
    int *p1 = &a;
    int *p2 = &b;
    [[cake::w1340]][[cake::w850]]
    int *p3 = p1 + p2; // Error: Cannot add two pointers
    return 0;
}
