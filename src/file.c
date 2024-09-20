
#include <stdlib.h>

#define SWAP(a, b) \
  do {\
    typeof(a) temp = a; a = b; b = temp; \
  } while (0)

#pragma expand SWAP

/*pointer to function returning int*/
int (*g1)(int a);
typeof(g1) g2;

/*function returning int* */
int * f1(int a);

/*function returning int* */
typeof(f1) f2;

/*pointer to function returning int*/
typeof(f1) *f3;


typeof(1 + 1) f()
{
  return 1;
}

void f4(int a[2]) {
    typeof(a) p;
}

int main()
{
    /*simple case*/
    int a = 1;
    typeof(a) b = 1;

    /*pay attention to the pointer*/
    typeof(int*) p1, p2;
    typeof(typeof(int*)) p3;
    typeof(typeof(p1)) p4;
    

    typeof_unqual(const int) p5;
    typeof_unqual(const int * const) p6;
    
    

    /*let's expand this macro and see inside*/
    SWAP(a, b);

    /*for anonymous structs we insert a tag*/
    struct { int i; } x;
    typeof(x) x2;
    typeof(x) x3;

   /*Things get a little more complicated*/
   int *array[2];
   typeof(array) a1, a2;
   
   typeof(array) a3[3];
   typeof(array) *a4[4];

   /*abstract declarator*/
   int k = sizeof(typeof(array));


   /*new way to declare pointer to functions?*/
   typeof(void (int)) * pf = NULL;
}


int f5(){
  typeof(int [2]) *p1 = 0;
  auto p2 = (typeof(int [2]) *) p1 ;
}


