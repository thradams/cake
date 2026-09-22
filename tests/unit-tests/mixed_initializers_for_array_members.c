// Test 10: Mixed initializers for array members

int arr[3] = {1, [2] = 5, 3}; //lint 62 excess elements in initializer

int main() {
    return 0;
}
