// Test 1: Applying & to a bit-field

struct BitField {
    int bit : 1;
};

int main() {
    struct BitField bf = {1}; //lint  74
    int *p = &bf.bit; //lint 1220 // Error: Cannot take the address of a bit-field
    return 0;
}
