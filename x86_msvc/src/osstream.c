/* Cake 0.12.26 x86_msvc */
struct __crt_locale_data;

struct __crt_multibyte_data;

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct osstream {
    char * c_str;
    int size;
    int capacity;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
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


static int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList);
int __cdecl __stdio_common_vsprintf(unsigned long long _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);
static unsigned long long *__cdecl __local_stdio_printf_options(void);
static unsigned long long __c0__OptionsStorage;
static int reserve(struct osstream * stream, int size);
void *realloc(void * ptr, unsigned int size);
int *__cdecl _errno(void);
static int __cdecl vsprintf(char * _Buffer, char * _Format, char * _ArgList);
static int __cdecl _vsnprintf_l(char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

int ss_vafprintf(struct osstream * stream, char * fmt, char * args)
{
    int size;
    char * tmpa;

    ;
    size = 0;
    _cake_zmem(&tmpa, 4);
    ((tmpa) = (args));
    size = vsnprintf(stream->c_str + stream->size, stream->capacity - stream->size, fmt, tmpa);
    ((void)(tmpa = (char *)0));
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



static unsigned long long *__cdecl __local_stdio_printf_options(void)
{
    return &__c0__OptionsStorage;
}

static int __cdecl vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList)
{
    int _Result;

    _Result = __stdio_common_vsprintf((*__local_stdio_printf_options()) | 2ULL, _Buffer, _BufferCount, _Format, 0, _ArgList);
    return _Result < 0 ? -1 : _Result;
}

static int reserve(struct osstream * stream, int size)
{
    int errorcode;

    errorcode = 0;
    if (size > stream->capacity)
    {
        void * pnew;

        pnew = realloc(stream->c_str, (size + 1) * 1);
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

static int __cdecl _vsnprintf_l(char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    int _Result;

    _Result = __stdio_common_vsprintf((*__local_stdio_printf_options()) | 1ULL, _Buffer, _BufferCount, _Format, _Locale, _ArgList);
    return _Result < 0 ? -1 : _Result;
}

static int __cdecl vsprintf(char * _Buffer, char * _Format, char * _ArgList)
{
    return _vsnprintf_l(_Buffer, 4294967295, _Format, 0, _ArgList);
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
    ((void)(args = (char *)(&(fmt)) + 4));
    size = ss_vafprintf(stream, fmt, args);
    ((void)(args = (char *)0));
    return size;
}


