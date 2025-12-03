int main()
{

    do
    {
        int i;
        if (i) break;
        defer i =1;

        int i2;
        if (i2) break;
        defer i = 2;


    } while (0);
}