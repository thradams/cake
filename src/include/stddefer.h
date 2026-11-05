/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

    #pragma once

    #define __STDC_VERSION_STDDEFER_H__ 202602L

    #define defer _Defer

#else
    #include_next <stddefer.h>
#endif
