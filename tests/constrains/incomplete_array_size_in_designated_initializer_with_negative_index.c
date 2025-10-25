// Test 9: Incomplete array size in designated initializer with negative index


int arr[] = {[-1] = 5}; // Error: Designator index cannot be negative

int main() {
    return 0;
}
