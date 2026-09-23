#include <stdio.h>

int main(void) {
    int a = 3, b = 4;

    int size = a * b; // runtime expression
    int arr[size]; // VLA

    for (int i = 0; i < size; i++)
    {
        arr[i] = i;
    }

    printf("Size = %d\n", size);
    for (int i = 0; i < size; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}
