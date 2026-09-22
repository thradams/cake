// Test 6: Using -> operator on a non-structure/union pointer

int main() {
    int x = 10;
    int *ptr = &x;
    ptr->y = 5; //lint 730 structure or union required
    return 0;
}
