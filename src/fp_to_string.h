/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once

/*
 *  Shortest decimal text that reads back as the exact same value - no
 *  precision is lost, and 0.1 stays "0.1" rather than 0.10000000000000001.
 *
 *  Both write a NUL-terminated string into buffer and return its length.
 *  The text always contains a '.' or an 'e', so it is a valid C floating
 *  constant and can never be read back as an integer one.
 *
 *  buffer must hold at least FP_TO_STRING_MIN_SIZE bytes; a smaller size
 *  writes an empty string and returns 0.
 *
 *  Only finite values are handled. Infinity and NaN have no literal form in
 *  C, so the caller has to emit those itself.
 */

#define FP_TO_STRING_MIN_SIZE 32

int double_to_string(double value, char* buffer, int size);
int float_to_string(float value, char* buffer, int size);
