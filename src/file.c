#include <stdio.h>

void f1(int array[5])
{
    int i = array[5];
    printf("%d", i);
}


void f2(int array[static 5])
{
    int i = array[5];
    printf("%d", i);
}

int main() {
    int array[5] = { 1, 2, 3, 4, 5 };
    int i = array[5];
    printf("%d", i);
    f1(array);
    f2(array);
    return 0;
}