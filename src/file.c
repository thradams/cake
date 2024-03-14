

#include <stdio.h>

int main()
{
    FILE* f = fopen("in.txt", "r");
    if (f != NULL)
    {
        defer fclose(f);

        FILE* f2 = fopen("out.txt", "w");
        if (f2 == NULL) goto LEND;
        defer fclose(f2);
    }
LEND:
    return 0;
}

