#include <stdio.h>

int main()
{
  static const char file_txt[] = {
   #embed "stdio.h"
   ,0
  };

  printf("%s\n", file_txt);

}

/* 
  Note
  The idea is to have a mode where cake generates a file to 
  be included like this:

  static const char file_txt[] = {
   #include "stdio.h.embed"
   ,0
  };

*/
