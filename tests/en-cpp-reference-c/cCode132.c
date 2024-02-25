//en.cppreference.com/w/c/language/operator_logical.html
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
int main(void)
{
    bool b = 2+2 == 4 || 2+2 == 5; // true
    printf("true or false = %s\n", b ? "true" : "false");
 
    // logical OR can be used simialar to perl's "or die", as long as rhs has scalar type
    fopen("test.txt", "r") || printf("could not open test.txt: %s\n", strerror(errno));
}