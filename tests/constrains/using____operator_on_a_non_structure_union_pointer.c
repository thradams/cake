// Test 6: Using -> operator on a non-structure/union pointer

int main() {
    int x = 10;
    int *ptr = &x;
    ptr->y = 5; // Error: ptr is not a pointer to a structure or union
    return 0;
}
