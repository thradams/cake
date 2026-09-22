// Test 3: Casting pointer to floating type


int main() {
    int *ptr = 0;
    float f = (float)ptr; //lint 1790 // Error: Cannot cast pointer to floating type
    return 0;
}
