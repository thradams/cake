// Test 5: Object declaration with incomplete type and no linkage


struct Incomplete;

struct Incomplete z; // Error: Object declaration with incomplete type and no linkage

int main() {
    return 0;
}
