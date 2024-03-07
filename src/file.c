
/*
  C23 introduced keyword bool as alternative to _Bool and
  true and false as constants.

  Cake translate bool to _Bool when compiling to C99/C11
  and to unsigned char when compiling to C89.
*/

//#include <stdio.h>


int main()
{
    bool b = true;
    b = false;
    static_assert(1 == true);
    static_assert(0 == false);


}
