//en.cppreference.com/w/c/language/sizeof.html
#include <stdio.h>
 
int main(void)
{
    short x;
    // type argument:
    printf("sizeof(float)          = %zu\n", sizeof(float));
    printf("sizeof(void(*)(void))  = %zu\n", sizeof(void(*)(void)));
    printf("sizeof(char[10])       = %zu\n", sizeof(char[10]));
//  printf("sizeof(void(void))     = %zu\n", sizeof(void(void))); // Error: function type
//  printf("sizeof(char[])         = %zu\n", sizeof(char[])); // Error: incomplete type
 
    // expression argument:
    printf("sizeof 'a'             = %zu\n", sizeof 'a'); // type of 'a' is int
//  printf("sizeof main            = %zu\n", sizeof main); // Error: Function type
    printf("sizeof &main           = %zu\n", sizeof &main);
    printf("sizeof \"hello\"         = %zu\n", sizeof "hello"); // type is char[6]
    printf("sizeof x               = %zu\n", sizeof x); // type of x is short
    printf("sizeof (x+1)           = %zu\n", sizeof(x + 1)); // type of x+1 is int
}