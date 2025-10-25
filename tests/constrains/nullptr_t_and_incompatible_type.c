// Test 8: nullptr_t and incompatible type

int main() {
    int cond = 1;
    
    int x = 5;
    void *result = cond ? nullptr : x; // Error: Cannot mix nullptr_t with an integer type
    return 0;
}
