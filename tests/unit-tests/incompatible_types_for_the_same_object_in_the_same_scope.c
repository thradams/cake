// Test 4: Incompatible types for the same object in the same scope


int y;
float y; //lint 1020 conflicting types

int main() {
    return 0;
}
