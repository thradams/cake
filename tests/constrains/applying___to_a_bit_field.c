// Test 1: Applying & to a bit-field

struct BitField {
    int bit : 1;
};

int main() {
    struct BitField bf = {1};
    int *p = &bf.bit; // Error: Cannot take the address of a bit-field
    return 0;
}
