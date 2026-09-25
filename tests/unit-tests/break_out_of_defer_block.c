int main(void)
{
    for (;;)
    {
        defer
        {
            break; //lint 1200 break cannot jump out of a defer block
        }
    }
    return 0;
}
