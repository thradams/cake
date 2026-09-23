#include <stdio.h>
int main()
{
  printf("%d", (static int (void) ){
    return 1;
  }());
}
