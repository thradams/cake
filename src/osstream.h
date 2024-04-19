#pragma once

#include <stdarg.h>
#include "ownership.h"

struct osstream
{
    char* owner opt c_str;
    int size;
    int capacity;
};


void ss_close( struct osstream * obj_owner stream);

int ss_vafprintf(struct osstream* stream, const char* fmt, va_list args);
int ss_fprintf(struct osstream* stream, const char* fmt, ...);
int ss_putc(char ch, struct osstream* stream);
void ss_clear(struct osstream* stream);
void ss_swap(struct osstream* a, struct osstream* b);
