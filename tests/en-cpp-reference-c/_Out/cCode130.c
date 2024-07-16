//en.cppreference.com/w/c/language/operator_logical.html
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
int main(void)
{
    bool b = !(2+2 == 4); // not true
    printf("!(2+2==4) = %s\n", b ? "true" : "false");
 
    int n = isspace('a'); // zero if 'a' is a space, nonzero otherwise
    int x = !!n; // "bang-bang", common C idiom for mapping integers to [0,1]
                 // (all non-zero values become 1)
    char *a[2] = {"nonspace", "space"};
    printf("%s\n", a[x]); // now x can be safely used as an index to array of 2 ints
}