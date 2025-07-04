struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void ss_swap(struct osstream * a, struct osstream * b)
{
    struct osstream  r;

    r = *a;
    *a = *b;
    *b = r;
}

void ss_clear(struct osstream * stream)
{
    if (stream->c_str)
    {
        stream->c_str[0] = 0;
    }
    stream->size = 0;
}

void free(void * ptr);

void ss_close(struct osstream * stream)
{
    free(stream->c_str);
}

void *realloc(void * ptr, unsigned int size);
int *__cdecl _errno(void);

static int reserve(struct osstream * stream, int size)
{
    int errorcode;

    errorcode = 0;
    if (size > stream->capacity)
    {
        void * pnew;

        pnew = realloc(stream->c_str, (size + 1) * 1U);
        if (pnew)
        {
            stream->c_str = pnew;
            stream->capacity = size;
            stream->c_str[size] = 0;
        }
        else
        {
            (*_errno()) = 12;
            errorcode = 1;
        }
    }
    return errorcode;
}


int __cdecl __stdio_common_vsprintf(unsigned __int64 _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

static unsigned __int64 __ck__OptionsStorage1;
inline unsigned __int64 *__cdecl __local_stdio_printf_options(void)
{
    return &__ck__OptionsStorage1;
}
inline int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList)
{
    int _Result;

    _Result = __stdio_common_vsprintf((*__local_stdio_printf_options()) | 2ULL, _Buffer, _BufferCount, _Format, 0U, _ArgList);
    return _Result < 0 ? -1 : _Result;
}


inline int __cdecl _vsnprintf_l(char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    int _Result;

    _Result = __stdio_common_vsprintf((*__local_stdio_printf_options()) | 1ULL, _Buffer, _BufferCount, _Format, _Locale, _ArgList);
    return _Result < 0 ? -1 : _Result;
}
inline int __cdecl vsprintf(char * _Buffer, char * _Format, char * _ArgList)
{
    return _vsnprintf_l(_Buffer, 4294967295U, _Format, 0U, _ArgList);
}

int ss_vafprintf(struct osstream * stream, char * fmt, char * args)
{
    int size;
    char * tmpa;

    ;
    size = 0;
    _cake_zmem(&tmpa, 4);
    ((tmpa) = (args));
    size = vsnprintf(stream->c_str + stream->size, stream->capacity - stream->size, fmt, tmpa);
    ((void)(tmpa = 0U));
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

int ss_putc(char ch, struct osstream * stream)
{
    if (reserve(stream, stream->size + 1) != 0)
    {
        return -1;
    }
    stream->c_str[stream->size] = ch;
    stream->size++;
    return ch;
}

int ss_fprintf(struct osstream * stream, char * fmt, ...)
{
    char * args;
    int size;

    _cake_zmem(&args, 4);
    ((void)(args = (char *)(&(fmt)) + 4U));
    size = ss_vafprintf(stream, fmt, args);
    ((void)(args = 0U));
    return size;
}

