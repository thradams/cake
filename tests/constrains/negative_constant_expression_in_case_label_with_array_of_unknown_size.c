// Test 8: Negative constant expression in case label with array of unknown size

void func() {
    int arr[]; // Array of unknown size
    int x = 1;
    switch (x) {
        case -1: // Error: Negative constant expression for array of unknown size
            //printf("Case -1\n");
            break;
        default:
            //printf("Default\n");
    }
}
