// Test 8: Both operands must have type nullptr_t

int main() {
    
    int x = 0;
    if (nullptr == x) { //lint 2270
        
    }
    return 0;
}
