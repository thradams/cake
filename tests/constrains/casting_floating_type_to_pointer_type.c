// Test 4: Casting floating type to pointer type


int main() {
    double d = 3.14;
    int *p = (int *)d; // Error: Cannot cast floating type to pointer type
    return 0;
}
