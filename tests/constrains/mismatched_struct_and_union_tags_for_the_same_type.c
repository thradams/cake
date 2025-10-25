// Test 1: Mismatched struct and union tags for the same type

struct MyType {
    int a;
};

union MyType { // Error: MyType cannot be redeclared as union
    int a;
    float b;
};

int main() {
    return 0;
}
