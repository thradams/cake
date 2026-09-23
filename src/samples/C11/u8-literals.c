/*
* cake input source code encode is always utf8
* cake output source code is also utf8

* u8 prefix may be useful in case you have a compiler where
* the input or output is not uft8.
*/

#include <stdio.h>

int main()
{
  printf("Hello, 世界\n");
  printf(u8"Hello, 世界\n");
}
