// Test 1: Redefinition of object-like macro with different replacement list

#define CONST 10
#define CONST 20 // Error: Redefinition of object-like macro with different replacement list

int main() {
    CONST;
    return 0;
}
