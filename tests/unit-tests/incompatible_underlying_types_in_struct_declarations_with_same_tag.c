// Test 9: Incompatible underlying types in struct declarations with same tag


struct AlignedStruct {
    int value;
};

struct AlignedStruct {
    float value;
}; //lint 2070 redefinition of struct with different content

int main() {
    return 0;
}
