// Test 3: Incompatible member types in struct declarations with the same tag

struct IncompatibleStruct {
    int x;
    float y;
};

struct IncompatibleStruct {
    float x;
    float y;
}; //lint 2070 redefinition of struct with different content

int main() {
    return 0;
}
