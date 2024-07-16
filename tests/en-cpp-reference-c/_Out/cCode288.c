//en.cppreference.com/w/c/preprocessor/line.html
#include <assert.h>
#define FNAME "test.c"
int main(void)
{
#line 777 FNAME
        assert(2+2 == 5);
}