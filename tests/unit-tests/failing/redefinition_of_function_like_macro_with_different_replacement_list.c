// Test 6: Redefinition of function-like macro with different replacement list

#define SQUARE(x) ((x) * (x))
#define SQUARE(x) ((x) + (x)) // Error: Redefinition of function-like macro with different replacement list

int main() {
    SQUARE(5);
    return 0;
}
