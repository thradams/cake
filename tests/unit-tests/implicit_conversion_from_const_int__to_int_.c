// Test 7: Implicit conversion from const int* to int*

int main() {
    const int x = 10;
    const int *const_ptr = &x;
    int *int_ptr = const_ptr; //lint 15 discarding const qualifier
    return 0;
}
