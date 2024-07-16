//en.cppreference.com/w/c/language/operator_member_access.html
#include <stdio.h>
struct s {int x;};
int main(void)
{
    struct s s={1}, *p = &s;
    p->x = 7; // changes the value of s.x through the pointer
    printf("%d\n", p->x); // prints 7
}