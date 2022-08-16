#pragma once
#include <stdarg.h>

struct osstream
{
    char* c_str;
    int size;
    int capacity;
};


int ss_close(struct osstream* stream);
int ss_vafprintf(struct osstream* stream, const char* fmt, va_list args);
int ss_fprintf(struct osstream* stream, const char* fmt, ...);
int ss_putc(char ch, struct osstream* stream);
void ss_clear(struct osstream* stream);
