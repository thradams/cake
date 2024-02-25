//en.cppreference.com/w/c/error/static_assert.html
#include <assert.h>
 
int main(void)
{
    static_assert(2 + 2 == 4, "2+2 isn't 4");   // well-formed
 
    static_assert(sizeof(int) < sizeof(char),   // compile-time error
                  "this program requires that int is less than char");
}