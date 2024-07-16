//en.cppreference.com/w/c/preprocessor/replace.html
#include <stdio.h>
 
// make function factory and use it
#define FUNCTION(name, a) int fun_##name(int x) { return (a) * x; }
 
FUNCTION(quadruple, 4)
FUNCTION(double, 2)
 
#undef FUNCTION
#define FUNCTION 34
#define OUTPUT(a) puts( #a )
 
int main(void)
{
    printf("quadruple(13): %d\n", fun_quadruple(13) );
    printf("double(21): %d\n", fun_double(21) );
    printf("%d\n", FUNCTION);
    OUTPUT(billion);               // note the lack of quotes
}