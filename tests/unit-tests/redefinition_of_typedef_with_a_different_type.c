// Test 3: Redefinition of typedef with a different type

typedef int myInt;
typedef float myInt; //lint 1020 conflicting types

int main() {
    return 0;
}
