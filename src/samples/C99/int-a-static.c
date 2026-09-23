#include <stdlib.h>


void F(int a[static const 5]) 
{
    //cake was a warning when modifying array arguments
    a = 0;

    //bounds check for constant indexes
    int k = a[5];
}


int main() {
    
    F(0);
    F(NULL);
    F(nullptr);

    int a[] = {1, 2, 3};    
    //F(a); /*try*/
    
    int b[] = { 1, 2, 3 , 4, 5};
    F(b);
    int k = b[5]; //bounds check

    int c[] = { 1, 2, 3 , 4, 5, 6};
    F(c);
}
