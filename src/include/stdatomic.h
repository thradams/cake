/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS
/* the generated code would need the atomic builtins of each compiler */
#error <stdatomic.h> is not available with -cake-headers yet
#else
#include_next <stdatomic.h>
#endif
