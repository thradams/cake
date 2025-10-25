// Test 6: Invalid cast to nullptr_t
typedef typeof(nullptr) nullptr_t;

int main() {
    int x = 5;
    nullptr_t p = (nullptr_t)x; // Error: Invalid cast to nullptr_t from non-null pointer constant
    return 0;
}
