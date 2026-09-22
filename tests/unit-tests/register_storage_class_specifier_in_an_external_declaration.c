// Test 1: register storage-class specifier in an external declaration

register int x; //lint 2180 'register' storage-class specifier cannot be used in a file-scope declaration

int main() {
    return 0;
}
