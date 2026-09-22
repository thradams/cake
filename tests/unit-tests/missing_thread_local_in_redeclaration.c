// Test 3: Missing thread_local in redeclaration

thread_local int z = 5;
int z; //lint 2300

int main() {
    return 0;
}
