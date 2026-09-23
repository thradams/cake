#pragma safety enable

#include <stdlib.h>
#include <string.h>

struct X {
  char *_Owner _Opt name;
};

struct X f(int condition)
{
   struct X x;
   if (condition) {
        x.name = strdup("a");
   }
   return x;
}

int main()
{
    struct X x = f(2);
}
