#pragma safety enable

void construct(_Out int* p, int cond)
{
    if (cond)
    {
        *p = 1;
    }
} //lint 71 'p' possibly not initialized when cond == 0

int main(int c)
{
    int y;
    construct(&y, c);
    return y;
}
