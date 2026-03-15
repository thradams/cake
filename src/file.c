
void fill(int n, int m, int (*grid)[n][m])
{
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            (*grid)[i][j] = i * 10 + j;

   sizeof(grid);
}

int main() {

}
