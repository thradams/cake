// Test 1: Compound literal with incomplete type


struct Incomplete;

int main() {
    (struct Incomplete){0}; //lint 740
    return 0;
}
