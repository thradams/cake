//en.cppreference.com/w/c/language/operator_logical.html
#include <stdbool.h>
#include <stdio.h>
int main(void)
{
    bool b = 2+2==4 && 2*2==4; // b == true
 
    1 > 2 && puts("this won't print");
 
    char *p = "abc";
    if(p && *p) // common C idiom: if p is not null
                // AND if p does not point at the end of the string
    {           // (note that thanks to short-circuit evaluation, this
                //  will not attempt to dereference a null pointer)
    // ...      // ... then do some string processing
    }
}