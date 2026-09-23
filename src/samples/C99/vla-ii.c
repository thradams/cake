/*
  VLA inside a block (lifetime demonstration)
*/

#include <stdio.h>

int main(void) {
    int n = 3;

    printf("Before block\n");

    {
        int arr[n]; // VLA with block scope

        for (int i = 0; i < n; i++) {
            arr[i] = i * i;
        }

        printf("Inside block:\n");
        for (int i = 0; i < n; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");
    }

    printf("After block\n");
    return 0;
}
