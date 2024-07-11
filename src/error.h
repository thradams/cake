/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once

#ifndef __CAKE__

#define try  
#define catch if (0) catch_label:
#define throw do { throw_break_point(); goto catch_label;}while (0)

#endif

const char* get_posix_error_message(int error);
int windows_error_to_posix(int i);

void throw_break_point();