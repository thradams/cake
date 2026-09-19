#pragma safety enable


void construct(_Out int* p)
{
} //lint 71 'p' is never initialized

int main()
{
    int y;
    construct(&y);
    return y;
}
