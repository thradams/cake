// Test 6: Mismatched struct and enum tags for the same type

struct MyType2 {
    int a;
};

enum MyType2 {
    A, B, C
}; //lint 1030 use of 'MyType2' with tag type that does not match previous declaration

int main() {
    return 0;
}
