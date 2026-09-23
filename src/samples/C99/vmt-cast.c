#include <stdio.h>
void test()
{
   int n = 1;
   typeof(int(*)[n++]) a;
   (int(*)[n++]) 0;
   (int(*)[n++]) 0;
   printf("%d", n);
}
//4

int main()
{
   test();

   int n = 1;
   int a[n];
    n = 2;
   auto p = (int(*)[n]) & a;
   sizeof(* p); //8
}
