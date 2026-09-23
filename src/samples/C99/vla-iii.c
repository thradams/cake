/*
VLA with 2D function parameter
*/
#include <stdio.h>

void print_matrix(int rows, int cols, int m[rows][cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", m[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    int r = 2, c = 3;

    int m[r][c]; // VLA

    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++)
            m[i][j] = i * c + j;

    print_matrix(r, c, m);
    return 0;
}
