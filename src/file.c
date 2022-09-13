

#include <stdio.h>

int main()
{

    do
    {
        FILE* f = fopen("in.txt", "r");
        if (f == NULL) break;
        defer fclose(f);

        FILE* f2 = fopen("out.txt", "w");
        if (f2 == NULL) break;
        defer fclose(f2);

        //...

       /*success here*/
    } while (0);


}

