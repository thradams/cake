/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake 
*/

#pragma once

#include <stdarg.h>
#include "ownership.h"

struct osstream
{
    char* _Owner _Opt c_str;
    int size;
    int capacity;
};


void ss_close( _Dtor struct osstream * stream);

int ss_vafprintf(struct osstream* stream, const char* fmt, va_list args);
int ss_fprintf(struct osstream* stream, const char* fmt, ...);
int ss_putc(char ch, struct osstream* stream);
void ss_clear(struct osstream* stream);
void ss_swap(struct osstream* a, struct osstream* b);
