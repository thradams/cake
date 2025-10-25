// Test 1: Compound literal with incomplete type


struct Incomplete;

int main() {
    (struct Incomplete){0}; // Error: Incomplete type for compound literal
    return 0;
}
