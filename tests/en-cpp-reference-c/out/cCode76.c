//en.cppreference.com/w/c/language/_Static_assert.html
#include <assert.h> // no longer needed since C23
 
int main(void)
{
    // Test if math works, C23:
    static_assert
    // Pre-C23 alternative:
    
 
    // This will produce an error at compile time.
    // static_assert(sizeof(int) < sizeof(char), "Unmet condition!");
 
    const int _42 = 2 * 3 * 2 * 3 + 2 * 3;
    static_assert // the message string can be omitted.
 
    // const int _13 = 13;
    // Compile time error - not an integer constant expression:
    // static_assert(_13 == 13);
}