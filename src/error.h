#pragma once
#include <stdarg.h>

#define try  if (1)
#define catch else catch_label:
#define throw goto catch_label


#ifndef _In_z_
#define _In_z_
#define _Printf_format_string_
#endif


const char* get_posix_error_message(int error);
int GetWindowsOrLinuxSocketLastErrorAsPosix(void);
int windows_error_to_posix(int i);
