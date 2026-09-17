/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/ 
#pragma once

#ifdef CAKE_HEADERS

#ifdef NDEBUG
#define assert(...) ((void)0)
#else
#define assert(...) assert(__VA_ARGS__)
#endif

#else
#include_next <assert.h>
#endif

