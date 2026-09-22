// Test 4: Union member with function type

union TestUnion {
    int value;
    int func(int x); //lint 1840 members having a function type are not allowed
};

int main() {
    return 0;
}
