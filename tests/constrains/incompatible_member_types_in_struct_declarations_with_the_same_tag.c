// Test 3: Incompatible member types in struct declarations with the same tag

struct IncompatibleStruct {
    int x;
    float y;
};

struct IncompatibleStruct {
    float x; // Error: Incompatible types for member 'x'
    float y;
};

int main() {
    return 0;
}
