// Test 10: Flexible array member in a union


union TestUnion {
    int size;
    int array[]; // Error: Flexible array member cannot be in a union
};

int main() {
    return 0;
}
