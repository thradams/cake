#pragma safety enable

int plus(int c)
{
    int a[4] = { 0 };
    int x = c ? 1 : 3;
    int y = +x;
    return a[x - y]; /* always 0 */
}

int plus_out_of_bounds(int c)
{
    int a[4] = { 0 };
    int x = c ? 1 : 3;
    int y = +x;
    return a[x - y - 1]; //lint 70
}
