/*
   Cake header file
*/


#pragma once

#ifdef CAKE_HEADERS

#define __bool_true_false_are_defined 1

#ifndef __cplusplus

#define bool  _Bool
#define false 0
#define true  1

#endif /* __cplusplus */


#else
#include_next <stdbool.h>
#endif


