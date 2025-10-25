// Test 9: Incompatible underlying types in struct declarations with same tag


struct AlignedStruct {
    int value;
};

struct AlignedStruct {
    float value; // Error: Incompatible types for member 'value'
};

int main() {
    return 0;
}
