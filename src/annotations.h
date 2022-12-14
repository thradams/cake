
#pragma once

enum static_analisys_flags
{
    ISVALID = 1 << 1,
    UNINITIALIZED = 1 << 2,
    MUST_DESTROY = 1 << 3,
    MUST_FREE = 1 << 4
};


#ifdef __CAKE__

#define _destroy [[destroy]]
#define _delete [[destroy,free]]
#define _free [[free]]

#else

#define _destroy 
#define _delete
#define _free

#define _del_attr(a, b)
#define _add_attr(a, b)

#endif
