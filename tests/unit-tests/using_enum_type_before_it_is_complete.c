// Test 4: Using enum type before it is complete

enum IncompleteEnum myVar; //lint 2200

enum IncompleteEnum {
    A, B, C
};

int main() {
    return 0;
}
