// Test 2: Initialization of an incomplete type


struct Incomplete;

struct Incomplete instance = {1}; // Error: Cannot initialize incomplete type

int main() {
    return 0;
}
