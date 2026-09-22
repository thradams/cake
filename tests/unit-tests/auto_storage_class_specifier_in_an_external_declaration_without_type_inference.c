// Test 2: auto storage-class specifier in an external declaration without type inference

auto int y; //lint 2170 'auto' storage-class specifier cannot be used in a file-scope declaration

int main() {
    return 0;
}
