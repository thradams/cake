/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

/* no include guard: assert may be redefined by including it again with a
   different NDEBUG */
#undef assert

#define __STDC_VERSION_ASSERT_H__ 202311L

#ifdef NDEBUG
#define assert(...) ((void)0)
#else
/* _Assert is the cake runtime assert: flow analysis assumes the condition
   holds after it (e.g. assert(p != NULL) makes p non null) */
#define assert(...) _Assert(__VA_ARGS__)
#endif

#if __STDC_VERSION__ < 202311L
#define static_assert _Static_assert
#endif

#else
#include_next <assert.h>
#endif
