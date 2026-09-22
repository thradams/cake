// Test 1: Using . operator on a non-structure/union type

int main() {
    int x = 10;
    x.member = 5; //lint 730 structure or union required
    return 0;
}
