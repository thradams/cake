// Test 4: Union member with function type

union TestUnion {
    int value;
    int func(int x); // Error: Union member cannot have a function type
};

int main() {
    return 0;
}
