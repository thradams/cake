
#include <stdio.h>

int main()
{
  FILE* f = NULL;
  _Defer if (f) fclose(f);

  do
  {
     f = fopen("in.txt", "r");
     if (f == NULL) break;     
  }
  while(0);

}
