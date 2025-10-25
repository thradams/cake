// Test 5: Switch statement not in scope of a variably modified type identifier
#include <stdio.h>

void func(int n) {
    int arr[n]; // Variably modified type
    switch (n) { // Error: Switch statement must be within the scope of variably modified type identifier
        case 1:
            printf("Case 1\n");
            break;
        default:
            printf("Default\n");
    }
}
