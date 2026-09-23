  //Case range expressions
  //https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3370.htm

  #include <stdio.h>

  void f(int n)
  {
    switch (n)
    {
       case 1 ... 10:
       printf("n in range 1...10\n");
       break;
       default:
       break;
    }
  }

  int main(){
    f(1);
    f(11);
  }
