// Test 8: Non-unique parameter in function-like macro definition

#define SUM(x, x) ((x) + (x)) // Error: Parameter names in macro definition must be unique

int main() {
    SUM(3, 4);
    return 0;
}
