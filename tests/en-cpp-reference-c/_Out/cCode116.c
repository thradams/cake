//en.cppreference.com/w/c/language/nullptr.html
#include <stddef.h>
#include <stdio.h>
 
void g(int*)
{
    puts("Function g called");
}
 
#define DETECT_NULL_POINTER_CONSTANT(e) \
    _Generic(e,                         \
        void* : puts("void*"),          \
        typeof(nullptr) : puts("nullptr_t"),  \
        default : puts("integer")       \
    )
 
int main()
{
    g(((void*)0)); // OK
    g(NULL); // OK
    g(0); // OK
 
    void  * cloned_nullptr = ((void*)0);
    g(cloned_nullptr); // OK
 
     void  * cloned_NULL = NULL;
//  g(cloned_NULL); // implementation-defined: maybe OK
 
     int cloned_zero = 0;
//  g(cloned_zero); // Error
 
    DETECT_NULL_POINTER_CONSTANT(((void*)0));
    DETECT_NULL_POINTER_CONSTANT(0);
    DETECT_NULL_POINTER_CONSTANT(nullptr);
    DETECT_NULL_POINTER_CONSTANT(NULL); // implementation-defined
}