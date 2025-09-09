/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once

typedef int errno_t;

#if __STDC_VERSION__  < 202311L 
#define _Attr(...)
#else
#define _Attr(...) [[ __VA_ARGS__ ]]
#endif


#ifndef __CAKE__

//emulate _Countof
#define _Countof(A) (sizeof(A)/sizeof((A)[0]))

#define try  
#define catch if (0) catch_label:
#define throw do { throw_break_point(); goto catch_label;}while (0)

#endif

const char* get_posix_error_message(int error);
int windows_error_to_posix(int i);

void throw_break_point();