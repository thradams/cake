// Test 1: Mismatched struct and union tags for the same type

struct MyType {
    int a;
};

union MyType {
    int a;
    float b;
}; //lint 1030 use of 'MyType' with tag type that does not match previous declaration

int main() {
    return 0;
}
