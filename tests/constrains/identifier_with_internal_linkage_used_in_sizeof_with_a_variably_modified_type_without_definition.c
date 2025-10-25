// Test 9: Identifier with internal linkage used in sizeof with a variably modified type without definition

int n = 10;
static int vla[n]; // Declaration without definition

int main() {
    int size = sizeof(vla); // Error: sizeof requires definition when used with a variably modified type
    return 0;
}
