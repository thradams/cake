// Test 5: Mixed void and non-void types

int main() {
    int cond = 1;
    void *v = 0;
    int x = 5;
    void *result = cond ? v : x; //lint 950 1340 incompatible types
    return 0;
}
