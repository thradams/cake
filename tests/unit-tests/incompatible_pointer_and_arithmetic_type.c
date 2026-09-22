// Test 7: Incompatible pointer and arithmetic type


int main() {
    int x = 10;
    int *ptr = &x;
    int result = ptr + x; //lint 75 pointer to integer conversion
    return 0;
}
