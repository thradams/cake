// Test 5: Non-static storage class in file scope compound literal

auto int x = (int){10}; // Error: auto storage class is invalid at file scope

int main() {
    return 0;
}
