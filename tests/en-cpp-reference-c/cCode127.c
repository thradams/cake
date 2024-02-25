//en.cppreference.com/w/c/language/operator_comparison.html
#include <assert.h>
int main(void)
{
    assert(1 < 2);
    assert(2+2 <= 4.0); // int converts to double, two 4.0's compare equal
 
    struct { int x,y; } s;
    assert(&s.x < &s.y); // struct members compare in order of declaration
 
    double d = 0.0/0.0; // NaN
    assert( !(d < d) );
    assert( !(d > d) );
    assert( !(d <= d) );
    assert( !(d >= d) );
    assert( !(d == d) );
 
    float f = 0.1; // f = 0.100000001490116119384765625
    double g = 0.1; // g = 0.1000000000000000055511151231257827021181583404541015625
    assert(f > g); // different values
}