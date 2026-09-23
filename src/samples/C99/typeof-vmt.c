#include <stdio.h>
int main()
{
    int n = 1;
    typeof(int (*)[n++]) p1;
    typeof(int (*)[n++]) p2;
    typeof(int (*)[n++]) p3;
    
    printf("%zu ", _Countof(*p1));

    printf("%zu ", _Countof(*p2));

    printf("%zu ", _Countof(*p3));
    
    printf("\n");
}
//1 2 3
