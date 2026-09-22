// Test 7: Duplicate typedef of a variably modified type


int n = 5;
typedef int array[n]; //lint 1890 variably modified type declaration not allowed at file scope
typedef int array[n]; //lint 1890 variably modified type declaration not allowed at file scope

int main() {
    return 0;
}
