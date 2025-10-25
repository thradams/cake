// Test 5: auto without file scope or type inference from initializer

int main() {
    auto int w; // Error: auto without inferred type from initializer in block scope
    return 0;
}
