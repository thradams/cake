// Test 3: Casting pointer to floating type


int main() {
    int *ptr = 0;
    float f = (float)ptr; // Error: Cannot cast pointer to floating type
    return 0;
}
