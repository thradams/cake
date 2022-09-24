
#include <stdlib.h>

#define SWAP(a, b) \
  do {\
    typeof(a) temp = a; a = b; b = temp; \
  } while (0)

#pragma expand SWAP

int main()
{
    /*simple case*/
    int a = 1;
    typeof(a) b = 1;

    /*pay attention to the pointer*/
    typeof(int*) p1, p2;

    /*let's expand this macro and see inside*/
    SWAP(a, b);

    /*for anonymous structs we insert a tag*/
    struct { int i; } x;
    typeof(x) x2;
    typeof(x) x3;

    /*Things get a little more complicated*/
    int* array[2];
    typeof(array) a1, a2;

    typeof(array) a3[3];
    typeof(array)* a4[4];

    /*abstract declarator*/
    int k = sizeof(typeof(array));


    /*new way to declare pointer to functions?*/
    typeof(void(int))* pf = NULL;
}

