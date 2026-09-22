// Test 8: nullptr_t and incompatible type

int main() {
    int cond = 1;
    
    int x = 5;
    void *result = cond ? nullptr : x; //lint 950 1340 incompatible types
    return 0;
}
