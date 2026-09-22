/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#define __bool_true_false_are_defined 1

#if __STDC_VERSION__ < 202311L
#define bool  _Bool
#define false 0
#define true  1
#endif

#else
#include_next <stdbool.h>
#endif
