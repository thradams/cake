// Test 6: Mismatched struct and enum tags for the same type

struct MyType2 {
    int a;
};

enum MyType2 { // Error: MyType2 cannot be redeclared as enum
    A, B, C
};

int main() {
    return 0;
}
