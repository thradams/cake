//en.cppreference.com/w/c/language/array.html
#include <stdio.h>
 
void f(int a[], int sz) // actually declares void f(int* a, int sz)
{
    for (int i = 0; i < sz; ++i)
        printf("%d\n", a[i]);
}
 
void g(int (*a)[10]) // pointer to array parameter is not transformed
{
    for (int i = 0; i < 10; ++i)
        printf("%d\n", (*a)[i]);
}
 
int main(void)
{
    int a[10] = {0};
    f(a, 10); // converts a to int*, passes the pointer
    g(&a);    // passes a pointer to the array (no need to pass the size)
}