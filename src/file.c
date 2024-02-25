
int main(void)
{
    int n = 1;
    int a[n]; // re-allocated 10 times, each with a different size
    int k = (sizeof a / sizeof * a);

    int b[2];
    int k2 = (sizeof b / sizeof * b);
}