#pragma once

#include <stdarg.h>
#include "annotations.h"

struct dtor osstream
{
    char* c_str;
    int size;
    int capacity;
};


void ss_close(struct osstream* stream);
int ss_vafprintf(struct osstream* stream, const char* fmt, va_list args);
int ss_fprintf(struct osstream* stream, const char* fmt, ...);
int ss_putc(char ch, struct osstream* stream);
void ss_clear(struct osstream* stream);

#ifdef __CAKE__
void ss_close(struct osstream* stream) extern {
    static_assert(_has_attr(stream, MUST_DESTROY));
    _del_attr(stream, MUST_DESTROY);
}
#endif

