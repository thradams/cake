
#pragma safety enable

#include <stdlib.h>

int main() {
   void * _Owner  _Opt p = malloc(1);
   if (p)
   {
     //free(p);
   }
   //static_debug(p);
}

