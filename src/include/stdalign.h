/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#if __STDC_VERSION__ < 202311L
#define alignas _Alignas
#define alignof _Alignof
#endif
#define __alignas_is_defined 1
#define __alignof_is_defined 1

#else
#include_next <stdalign.h>
#endif
