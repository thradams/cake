
/*
 * Cake compatibility header. https://github.com/thradams/cake
 *
 * This header provides empty definitions for Cake-specific annotations
 * and analysis directives when the source is compiled by a C compiler
 * other than Cake. This allows the same source code to be compiled
 * without requiring those compilers to understand Cake extensions.
 */

#pragma once

#ifndef __CAKE__

    #define _Out
    #define _Opt
    #define _Owner
    #define _Dtor
    #define _View
    #define _Clear
    #define _Uninitialized
    #define static_debug(x)
    #define override_state(x, s)
    #define _Assert(x) ((void)0)

#endif
