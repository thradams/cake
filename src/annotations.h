
#pragma once

enum static_analisys_flags
{
    ISVALID = 1 << 1,
    UNINITIALIZED = 1 << 2,
    MUST_DESTROY = 1 << 3,
    MUST_FREE = 1 << 4
};


#ifdef __CAKE__

#define dtor [[nodiscard]]
#define check(...) static_assert(__VA_ARGS__)

#else

#define dtor
#define _del_attr(a, b)
#define _add_attr(a, b)
#define check(a) 

#endif
