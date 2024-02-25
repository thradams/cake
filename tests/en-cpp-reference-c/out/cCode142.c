//en.cppreference.com/w/c/language/scalar_initialization.html
#include <stdbool.h>
int main(void)
{
    bool b = true;
    const double d = 3.14;
    int k = 3.15; // conversion from double to int
    int n = {12}, // optional braces
       *p = &n,   // non-constant expression OK for automatic variable
       (*fp)(void) = main;
    enum {RED, BLUE} e = RED; // enumerations are scalar types as well
}