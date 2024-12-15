struct __crt_multibyte_data;
struct osstream {
    char * c_str;
    int size;
    int capacity;
};

struct __crt_locale_pointers {
    struct __crt_locale_data * locinfo;
    struct __crt_multibyte_data * mbcinfo;
};

struct __crt_locale_data;

void ss_swap(struct osstream * a, struct osstream * b)
{
    struct osstream  r =  *a;
     *a =  *b;
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
int *_errno(void);

static int reserve(struct osstream * stream, int size)
{
    int errorcode = 0;
    if (size > stream->capacity)
    {
        void * pnew = realloc(stream->c_str, (size + 1) * sizeof (char));
        if (pnew)
        {
            stream->c_str = pnew;
            stream->capacity = size;
            stream->c_str[size] = 0;
        }
        else
        {
            ( *_errno()) = 12;
            errorcode = 1;
        }
    }
    return errorcode;
}


int __stdio_common_vsprintf(unsigned __int64 _Options, char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList);

inline unsigned __int64 *__local_stdio_printf_options(void)
{
    static unsigned __int64 _OptionsStorage;
    return &_OptionsStorage;
}
inline int vsnprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, char * _ArgList)
{
    int _Result = __stdio_common_vsprintf(( *__local_stdio_printf_options()) | (1ULL << 1), _Buffer, _BufferCount, _Format, ((void *)0), _ArgList);
    return _Result < 0 ?  -1 : _Result;
}


inline int _vsnprintf_l(char * _Buffer, unsigned int _BufferCount, char * _Format, struct __crt_locale_pointers * _Locale, char * _ArgList)
{
    int _Result = __stdio_common_vsprintf(( *__local_stdio_printf_options()) | (1ULL << 0), _Buffer, _BufferCount, _Format, _Locale, _ArgList);
    return _Result < 0 ?  -1 : _Result;
}
inline int vsprintf(char * _Buffer, char * _Format, char * _ArgList)
{
    return _vsnprintf_l(_Buffer, (unsigned int) -1, _Format, ((void *)0), _ArgList);
}

int ss_vafprintf(struct osstream * stream, char * fmt, char * args)
{
    ;
    int size = 0;
    char * tmpa = {0};
    ((tmpa) = (args));
    size = vsnprintf(stream->c_str + stream->size, stream->capacity - stream->size, fmt, tmpa);
    ((void)(tmpa = (char *)0));
    if (size < 0)
    {
        return  -1;
    }
    if (reserve(stream, stream->size + size) != 0)
    {
        return  -1;
    }
    size = vsprintf(stream->c_str + stream->size, fmt, args);
    if (size > 0)
    {
        stream->size = size;
    }
    return size;
}

int ss_putc(char ch, struct osstream * stream)
{
    if (reserve(stream, stream->size + 1) != 0)
    {
        return ( -1);
    }
    stream->c_str[stream->size] = ch;
    stream->size++;
    return ch;
}

int ss_fprintf(struct osstream * stream, char * fmt, ...)
{
    char * args = {0};
    ((void)(args = (char *)(&(fmt)) + ((sizeof (fmt) + sizeof (int) - 1) &  ~(sizeof (int) - 1))));
    int size = ss_vafprintf(stream, fmt, args);
    ((void)(args = (char *)0));
    return size;
}

