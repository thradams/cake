#include <stdio.h>
int main()
{
    int n = 1;
    printf("%zu ", sizeof(int [n++]));

    printf("%zu ", sizeof(int [n++]));

    printf("%zu ", sizeof(int [n++]));

}