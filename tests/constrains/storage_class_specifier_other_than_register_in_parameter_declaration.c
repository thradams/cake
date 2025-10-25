// Test 3: Storage-class specifier other than register in parameter declaration

void invalid_storage_class(static int x) { // Error: Only register storage class is allowed in parameter declarations
}
