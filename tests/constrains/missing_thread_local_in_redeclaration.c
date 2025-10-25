// Test 3: Missing thread_local in redeclaration

thread_local int z = 5;
int z; // Error: Missing thread_local in redeclaration

int main() {
    return 0;
}
