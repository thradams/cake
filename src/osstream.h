#pragma once

#include <stdarg.h>
#include "annotations.h"

struct _destroy osstream
{
    char* c_str;
    int size;
    int capacity;
};


void ss_close(_destroy struct osstream* stream);
int ss_vafprintf(struct osstream* stream, const char* fmt, va_list args);
int ss_fprintf(struct osstream* stream, const char* fmt, ...);
int ss_putc(char ch, struct osstream* stream);
void ss_clear(struct osstream* stream);

