// Test 7: Implicit pointer conversion without explicit cast

int main() {
    int x = 10;
    int *ptr = x; // Error: Implicit conversion of int to pointer without explicit cast
    return 0;
}
