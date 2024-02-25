//en.cppreference.com/w/c/language/operator_assignment.html
#include <stdio.h>
 
int main(void)
{
    int x = 10; 
    int hundred = 100; 
    int ten = 10; 
    int fifty = 50; 
 
    printf("%d %d %d %d\n", x, hundred, ten, fifty);
 
    hundred *= x; 
    ten     /= x; 
    fifty   %= x; 
 
    printf("%d %d %d %d\n", x, hundred, ten, fifty);
 
    return 0;
}