// Test 4: Implicit conversion from double* to char*

int main() {
    double d = 3.14;
    double *double_ptr = &d;
    char *char_ptr = double_ptr; //lint 54 incompatible types
    return 0;
}
