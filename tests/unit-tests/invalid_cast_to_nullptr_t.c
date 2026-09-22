// Test 6: Invalid cast to nullptr_t
typedef typeof(nullptr) nullptr_t;

int main() {
    int x = 5;
    nullptr_t p = (nullptr_t)x; //lint 1810 cannot cast this expression to nullptr_t
    return 0;
}
