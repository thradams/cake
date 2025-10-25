// Test 7: Duplicate typedef of a variably modified type


int n = 5;
typedef int array[n];
typedef int array[n]; // Error: Duplicate typedef of a variably modified type

int main() {
    return 0;
}
