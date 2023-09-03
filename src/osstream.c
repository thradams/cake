#include <stdbool.h>
#include "osstream.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "ownership.h"

void ss_swap(view struct osstream* a, view struct osstream* b)
{
    view struct osstream r = *a;
    *a = *b;
    *b = r;    
}

void ss_clear(struct osstream* stream)
{
    stream->size = 0;
}


void ss_close( struct osstream * obj_owner stream)
{
    free(stream->c_str);    
}

static int reserve(struct osstream* stream, int size)
{
    int errorcode = 0;
    if (size > stream->capacity)
    {
        void* owner pnew = realloc( stream->c_str, (size + 1) * sizeof(char));
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
    va_list tmpa;

    va_copy(tmpa, args);

    size = vsnprintf(stream->c_str + stream->size, stream->capacity - stream->size, fmt, tmpa);

    va_end(tmpa);

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
    va_list args;
    va_start(args, fmt);
    int size = ss_vafprintf(stream, fmt, args);
    va_end(args);
    return size;
}

