// Test 2: Variably modified type in a generic association
#include <stdio.h>

#define GENERIC_EXAMPLE(x, n) _Generic((x), int[n]: "int array", default: "default")

int main() {
    int n = 5;
    int arr[n]; // Variable-length array (VLA)
    printf("%s\n", GENERIC_EXAMPLE(arr, n)); // Should flag error due to VLA type
    return 0;
}
