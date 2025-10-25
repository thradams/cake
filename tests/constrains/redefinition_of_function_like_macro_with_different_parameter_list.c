// Test 2: Redefinition of function-like macro with different parameter list

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAX(x, y, z) ((x) > (y) ? (x) : (y)) // Error: Redefinition of function-like macro with different parameter list

int main() {
    MAX(1, 2);
    return 0;
}
