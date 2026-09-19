/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS
/* only glibc >= 2.39 exports the stdc_* functions */
#error <stdbit.h> is not available with -cake-headers yet
#else
#include_next <stdbit.h>
#endif
