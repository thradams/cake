#include <stdio.h>

void sizeof_sample()
{
    int n = 1;
    printf("%zu ", sizeof(int [n++]));

    printf("%zu ", sizeof(int [n++]));

    printf("%zu ", sizeof(int [n++]));

    printf("\n");
}

void countof_sample()
{
    int n = 1;
    printf("%zu ", _Countof(int [n++]));

    printf("%zu ", _Countof(int [n++]));

    printf("%zu ", _Countof(int [n++]));

    printf("\n");
}

int main(){
    sizeof_sample();
    countof_sample();
}

// 4 8 12 
// 1 2 3
