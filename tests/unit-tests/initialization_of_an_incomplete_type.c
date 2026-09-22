// Test 2: Initialization of an incomplete type


struct Incomplete;

struct Incomplete instance = {1}; //lint 740

int main() {
    return 0;
}
