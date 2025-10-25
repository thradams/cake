// Test 3: Redefinition of typedef with a different type

typedef int myInt;
typedef float myInt; // Error: Redefinition of typedef with a different type

int main() {
    return 0;
}
