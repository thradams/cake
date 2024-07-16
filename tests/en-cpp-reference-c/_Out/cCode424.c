//en.cppreference.com/w/c/types/nullptr_t.html
#include <stddef.h>
#include <stdio.h>
 
#define DETECT_NULL_POINTER_CONSTANT(e) \
    _Generic(e,                         \
        void* : puts("void*"),          \
        typeof(nullptr) : puts("nullptr_t"),  \
        default : puts("other")       \
    )
 
int main()
{
    DETECT_NULL_POINTER_CONSTANT(((void*)0));
    DETECT_NULL_POINTER_CONSTANT(0);
    DETECT_NULL_POINTER_CONSTANT(nullptr);
}