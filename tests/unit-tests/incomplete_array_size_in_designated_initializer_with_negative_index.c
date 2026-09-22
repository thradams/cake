// Test 9: Incomplete array size in designated initializer with negative index


int arr[] = {[-1] = 5}; //lint 720 array designator value is negative

int main() {
    return 0;
}
