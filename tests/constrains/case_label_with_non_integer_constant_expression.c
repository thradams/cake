// Test 2: Case label with non-integer constant expression

int main() {
    int x = 2;
    int y = 3;
    switch (x) {
        case y: // Error: case label must be an integer constant expression
            
            break;
        default:
            break;
    }
    return 0;
}
