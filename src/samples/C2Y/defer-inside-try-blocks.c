/*
  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3199.htm
*/

#include <stdio.h>

int main()
{

  try
  {
     FILE* f = fopen("in.txt", "r");
     if (f == NULL) throw;
     _Defer fclose(f);

     FILE* f2 = fopen("out.txt", "w");
     if (f2 == NULL) throw;
     _Defer fclose(f2);

     //...

    /*success here*/
  }
  catch
  {
     /*some error*/
  }


}
