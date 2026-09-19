/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#define noreturn _Noreturn

#else
#include_next <stdnoreturn.h>
#endif
