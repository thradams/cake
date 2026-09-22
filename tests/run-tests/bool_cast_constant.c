/*
   6.3.1.2: conversion to bool yields 0 only when the value compares equal
   to 0. The constant folder used to truncate the value to the storage type
   of bool (unsigned char), so (bool)256 was folded to 0.
*/
#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

static_assert((bool)256 == 1);
static_assert((bool)-256 == 1);
static_assert((bool)65536 == 1);
static_assert((bool)256.0 == 1);
static_assert((bool)0 == 0);
static_assert((bool)0.0 == 0);
static_assert((bool)(_BitInt(9))256 == 1);            /* wraps to -256 */
static_assert((bool)(unsigned _BitInt(8))256 == 0);   /* wraps to 0 */

int main(void)
{
    bool a = (bool)256;
    bool b = (bool)-256;
    bool c = (bool)0x100000000;
    bool d = (bool)(_BitInt(9))256;
    bool e = (bool)(unsigned _BitInt(8))256;
    int x = 256;
    bool f = (bool)x;

    CHECK(a == 1);
    CHECK(b == 1);
    CHECK(c == 1);
    CHECK(d == 1);
    CHECK(e == 0);
    CHECK(f == 1);

    return failures;
}
