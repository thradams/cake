#pragma once

#include <stdarg.h>
#include "annotations.h"

struct _owner osstream
{
    char* _owner c_str;
    int size;
    int capacity;
};


void ss_close(implicit struct osstream * _obj_owner stream);
char * _owner ss_get_str_and_close(implicit struct osstream * _obj_owner stream);
int ss_vafprintf(struct osstream* stream, const char* fmt, va_list args);
int ss_fprintf(struct osstream* stream, const char* fmt, ...);
int ss_putc(char ch, struct osstream* stream);
void ss_clear(struct osstream* stream);
void ss_swap(struct osstream* a, struct osstream* b);
