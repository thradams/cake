// Test 1: restrict qualifier on a non-pointer, non-array type

int restrict x = 10; //lint 2230

int main() {
    return 0;
}
