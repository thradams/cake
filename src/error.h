#pragma once
#include <stdarg.h>

#define try  if (1)
#define catch else catch_label:
#define throw goto catch_label

struct error
{
    char message[200];
    int code;
};

/*
  The seterror function returns the number of characters that are written,
  not counting the terminating null character.

  If the buffer size specified by er->capacity isn't sufficiently large to contain
  the output specified by string_format and argptr, the return value of seterror is the
  number of characters that would be written, not counting the null character,
  if er->capacity were sufficiently large.

  If the return value is greater than count - 1, the output has been truncated.

  A return value of -1 indicates that an encoding error has occurred.
*/

#ifndef _In_z_
#define _In_z_
#define _Printf_format_string_
#endif

int seterror(struct error* er, _In_z_ _Printf_format_string_  const char* fmt, ...);



void clearerror(struct error* err);

const char* get_posix_error_message(int error);
int GetWindowsOrLinuxSocketLastErrorAsPosix(void);
int windows_error_to_posix(int i);