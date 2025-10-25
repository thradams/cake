// Test 7: Invalid storage-class specifier in function prototype scope

void func(static int arr[5]) { // Error: Invalid storage class for compound literal in prototype scope
}
