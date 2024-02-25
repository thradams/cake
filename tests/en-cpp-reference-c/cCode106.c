//en.cppreference.com/w/c/language/file_scope.html
#include <stdio.h>
 
int a = 1;
static int b = 2;
 
void f (void) {printf("from function f()\n");}
static void g (void) {printf("from function g()\n");}
 
int main(void)
{
    f();
    g();
 
    return 0;
}
/* end of this translation unit, end of file scope */