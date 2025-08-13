/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake 
*/

#pragma safety enable

#include <stdbool.h>
#include "osstream.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "ownership.h"

void ss_swap(_View struct osstream* a, _View struct osstream* b)
{
    _View struct osstream r = *a;
    *a = *b;
    *b = r;
}

void ss_clear(struct osstream* stream)
{
    if (stream->c_str)
        stream->c_str[0] = '\0';
    stream->size = 0;
}


void ss_close(_Dtor struct osstream* stream)
{
    free(stream->c_str);
}

static int reserve(struct osstream* stream, int size)
{
    int errorcode = 0;
    if (size > stream->capacity)
    {
        void* _Owner _Opt pnew = realloc(stream->c_str, (size + 1) * sizeof(char));
        if (pnew)
        {
            static_set(stream->c_str, "moved");
            stream->c_str = pnew;
            stream->capacity = size;
            stream->c_str[size] = 0;
        }
        else
        {
            errno = ENOMEM;
            errorcode = 1;
        }
    }
    return errorcode;
}

int ss_vafprintf(struct osstream* stream, const char* fmt, va_list args)
{
    assert(fmt != 0);
    int size = 0;

#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "-Wnullable-to-non-nullable"
#pragma CAKE diagnostic ignored "-Wanalyzer-null-dereference"



    va_list tmpa = { 0 };

    va_copy(tmpa, args);

    size = vsnprintf(stream->c_str + stream->size, stream->capacity - stream->size, fmt, tmpa);

    va_end(tmpa);

#pragma CAKE diagnostic pop

    if (size < 0)
    {
        return -1;
    }

    if (reserve(stream, stream->size + size) != 0)
    {
        return -1;
    }

    size = vsprintf(stream->c_str + stream->size, fmt, args);
    if (size > 0)
    {
        stream->size += size;
    }
    return size;
}

/*
* Returns the character written. For fputc, a return value of EOF indicates an error
*/
int ss_putc(char ch, struct osstream* stream)
{
    if (reserve(stream, stream->size + 1) != 0)
    {
        return EOF;
    }
    stream->c_str[stream->size] = ch;
    stream->size++;

    return ch;
}

int ss_fprintf(struct osstream* stream, const char* fmt, ...)
{
#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "-Wnullable-to-non-nullable"
#pragma CAKE diagnostic ignored "-Wanalyzer-null-dereference"

    va_list args = { 0 };
    va_start(args, fmt);
    int size = ss_vafprintf(stream, fmt, args);
    va_end(args);

#pragma CAKE diagnostic pop

    return size;
}

