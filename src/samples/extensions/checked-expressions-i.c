#include <stdio.h>

int main()
{
    try
    {
        FILE* f = fopen("in.txt", "r") !;
        defer fclose(f);
    }
    catch
    {
    }
}
