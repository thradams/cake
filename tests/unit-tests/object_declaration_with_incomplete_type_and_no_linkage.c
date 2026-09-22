// Test 5: Object declaration with incomplete type and no linkage


struct Incomplete;

struct Incomplete z; //lint 740

int main() {
    return 0;
}
