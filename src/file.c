
int main()
{
    int i;
    defer if (i) i = 1;

    try
    {
        throw;
    }
    catch
    {
    }

}

