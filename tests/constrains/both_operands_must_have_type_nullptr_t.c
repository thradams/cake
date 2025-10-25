// Test 8: Both operands must have type nullptr_t

int main() {
    
    int x = 0;
    if (nullptr == x) { // Error: Cannot compare nullptr_t with integer
        
    }
    return 0;
}
