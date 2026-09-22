// Test 7: Implicit pointer conversion without explicit cast

int main() {
    int x = 10;
    int *ptr = x; //lint 1340 non-pointer to pointer
    return 0;
}
