#include <stdio.h>
typedef typeof(nullptr) nullptr_t;

void g(int*)
{
    puts("Function g called");
}
 
#define DETECT_NULL_POINTER_CONSTANT(e) \
    _Generic(e,                         \
        void* : puts("void*"),          \
        nullptr_t : puts("nullptr_t"),  \
        default : puts("integer")       \
    )
 
int main()
{
    g(nullptr);         // Fine
    g(NULL);            // Fine
    g(0);               // Fine
 
    puts("-----------------");
 
    auto cloned_nullptr = nullptr;
    auto cloned_NULL    = NULL;
    auto cloned_zero    = 0;
    g(cloned_nullptr);  // Fine
//  g(cloned_NULL);     // ERROR
//  g(cloned_zero);     // ERROR
 
    puts("-----------------");
 
    DETECT_NULL_POINTER_CONSTANT(((void*)0));
    DETECT_NULL_POINTER_CONSTANT(0);
    DETECT_NULL_POINTER_CONSTANT(nullptr);
    DETECT_NULL_POINTER_CONSTANT(NULL); // implementation-defined
}