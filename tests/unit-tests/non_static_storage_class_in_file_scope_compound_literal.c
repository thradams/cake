// Test 5: Non-static storage class in file scope compound literal

auto int x = (int){10}; //lint 2170 1860 auto storage-class specifier cannot be used in a file-scope declaration

int main() {
    return 0;
}
