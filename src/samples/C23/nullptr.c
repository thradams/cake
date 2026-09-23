
#include <stdlib.h>
#include <stdio.h>

int main()
{
  void * p = nullptr;
  void * p2 = NULL;

  auto a = nullptr;
  
  printf("%s", _Generic(nullptr, typeof(nullptr) : "nullptr_t"));
}
