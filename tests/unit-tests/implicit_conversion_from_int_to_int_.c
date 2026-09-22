// Test 1: Implicit conversion from int to int*


int main() {
    int x = 10;
    int *ptr = x; //lint 1340 non-pointer to pointer
    return 0;
}
