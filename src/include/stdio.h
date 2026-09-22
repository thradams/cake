/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#include <__cake_types.h>

#define __STDC_VERSION_STDIO_H__ 202311L

typedef __cake_size_t size_t;
typedef __cake_va_list va_list;
typedef __cake_fpos_t fpos_t;

/* opaque, the real struct is only known by the system headers */
typedef struct __cake_FILE FILE;

#ifndef NULL
#define NULL ((void*)0)
#endif

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#define EOF (-1)

#if defined(_WIN32)
#define BUFSIZ       512
#define FOPEN_MAX    20
#define FILENAME_MAX 260
#define L_tmpnam     260
#define TMP_MAX      2147483647
#elif defined(__APPLE__)
#define BUFSIZ       1024
#define FOPEN_MAX    20
#define FILENAME_MAX 1024
#define L_tmpnam     1024
#define TMP_MAX      308915776
#elif defined(__linux__)
#define BUFSIZ       8192
#define FOPEN_MAX    16
#define FILENAME_MAX 4096
#define L_tmpnam     20
#define TMP_MAX      238328
#else
#define BUFSIZ       512
#define FOPEN_MAX    8
#define FILENAME_MAX 260
#define L_tmpnam     260
#define TMP_MAX      32767
#endif

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

/* the standard streams are macros over the real libc objects */
#if defined(_WIN32)
FILE* __acrt_iob_func(unsigned index);
#define stdin  (__acrt_iob_func(0))
#define stdout (__acrt_iob_func(1))
#define stderr (__acrt_iob_func(2))
#elif defined(__APPLE__)
extern FILE* __stdinp;
extern FILE* __stdoutp;
extern FILE* __stderrp;
#define stdin  __stdinp
#define stdout __stdoutp
#define stderr __stderrp
#else
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;
#endif

/* operations on files */
int remove(const char* filename);
int rename(const char* old, const char* new);
FILE* _Owner _Opt tmpfile(void);
char* _Opt tmpnam(char* _Opt s);

/* file access */
int fclose(FILE* _Owner stream);
int fflush(FILE* _Opt stream);
FILE* _Owner _Opt fopen(const char* restrict filename, const char* restrict mode);
FILE* _Owner _Opt freopen(const char* _Opt restrict filename, const char* restrict mode, FILE* _Owner restrict stream);
void setbuf(FILE* restrict stream, char* _Opt restrict buf);
int setvbuf(FILE* restrict stream, char* _Opt restrict buf, int mode, size_t size);

/* formatted input/output */
#if defined(_WIN32)

/*
  The msvc CRT does not export the printf/scanf family: its headers define
  them inline over __stdio_common_v* (both x86 and x64), so we do the same.
  The option bits are those of corecrt_stdio_config.h.
*/
#include <stdarg.h>

#define __cake_PRINTF_LEGACY_VSPRINTF_NULL_TERMINATION (1ULL << 0)
#define __cake_PRINTF_STANDARD_SNPRINTF_BEHAVIOR       (1ULL << 1)

int __stdio_common_vfprintf(unsigned long long options, FILE* stream, const char* format, void* _Opt locale, va_list arg);
int __stdio_common_vsprintf(unsigned long long options, char* _Opt s, size_t n, const char* format, void* _Opt locale, va_list arg);
int __stdio_common_vfscanf(unsigned long long options, FILE* stream, const char* format, void* _Opt locale, va_list arg);
int __stdio_common_vsscanf(unsigned long long options, const char* s, size_t n, const char* format, void* _Opt locale, va_list arg);

static inline int vfprintf(FILE* restrict stream, const char* restrict format, va_list arg)
{
    return __stdio_common_vfprintf(0, stream, format, NULL, arg);
}

static inline int vprintf(const char* restrict format, va_list arg)
{
    return __stdio_common_vfprintf(0, stdout, format, NULL, arg);
}

static inline int vsprintf(char* restrict s, const char* restrict format, va_list arg)
{
    int r = __stdio_common_vsprintf(__cake_PRINTF_LEGACY_VSPRINTF_NULL_TERMINATION, s, (size_t)-1, format, NULL, arg);
    return r < 0 ? -1 : r;
}

static inline int vsnprintf(char* _Opt restrict s, size_t n, const char* restrict format, va_list arg)
{
    int r = __stdio_common_vsprintf(__cake_PRINTF_STANDARD_SNPRINTF_BEHAVIOR, s, n, format, NULL, arg);
    return r < 0 ? -1 : r;
}

static inline int vfscanf(FILE* restrict stream, const char* restrict format, va_list arg)
{
    return __stdio_common_vfscanf(0, stream, format, NULL, arg);
}

static inline int vscanf(const char* restrict format, va_list arg)
{
    return __stdio_common_vfscanf(0, stdin, format, NULL, arg);
}

static inline int vsscanf(const char* restrict s, const char* restrict format, va_list arg)
{
    return __stdio_common_vsscanf(0, s, (size_t)-1, format, NULL, arg);
}

static inline int fprintf(FILE* restrict stream, const char* restrict format, ...)
{
    va_list arg;
    va_start(arg, format);
    int r = vfprintf(stream, format, arg);
    va_end(arg);
    return r;
}

static inline int printf(const char* restrict format, ...)
{
    va_list arg;
    va_start(arg, format);
    int r = vprintf(format, arg);
    va_end(arg);
    return r;
}

static inline int sprintf(char* restrict s, const char* restrict format, ...)
{
    va_list arg;
    va_start(arg, format);
    int r = vsprintf(s, format, arg);
    va_end(arg);
    return r;
}

static inline int snprintf(char* _Opt restrict s, size_t n, const char* restrict format, ...)
{
    va_list arg;
    va_start(arg, format);
    int r = vsnprintf(s, n, format, arg);
    va_end(arg);
    return r;
}

static inline int fscanf(FILE* restrict stream, const char* restrict format, ...)
{
    va_list arg;
    va_start(arg, format);
    int r = vfscanf(stream, format, arg);
    va_end(arg);
    return r;
}

static inline int scanf(const char* restrict format, ...)
{
    va_list arg;
    va_start(arg, format);
    int r = vscanf(format, arg);
    va_end(arg);
    return r;
}

static inline int sscanf(const char* restrict s, const char* restrict format, ...)
{
    va_list arg;
    va_start(arg, format);
    int r = vsscanf(s, format, arg);
    va_end(arg);
    return r;
}

#else

int fprintf(FILE* restrict stream, const char* restrict format, ...);
int fscanf(FILE* restrict stream, const char* restrict format, ...);
int printf(const char* restrict format, ...);
int scanf(const char* restrict format, ...);
int snprintf(char* _Opt restrict s, size_t n, const char* restrict format, ...);
int sprintf(char* restrict s, const char* restrict format, ...);
int sscanf(const char* restrict s, const char* restrict format, ...);
int vfprintf(FILE* restrict stream, const char* restrict format, va_list arg);
int vfscanf(FILE* restrict stream, const char* restrict format, va_list arg);
int vprintf(const char* restrict format, va_list arg);
int vscanf(const char* restrict format, va_list arg);
int vsnprintf(char* _Opt restrict s, size_t n, const char* restrict format, va_list arg);
int vsprintf(char* restrict s, const char* restrict format, va_list arg);
int vsscanf(const char* restrict s, const char* restrict format, va_list arg);

#endif

/* character input/output */
int fgetc(FILE* stream);
char* _Opt fgets(char* restrict s, int n, FILE* restrict stream);
int fputc(int c, FILE* stream);
int fputs(const char* restrict s, FILE* restrict stream);
int getc(FILE* stream);
int getchar(void);
int putc(int c, FILE* stream);
int putchar(int c);
int puts(const char* s);
int ungetc(int c, FILE* stream);

/* direct input/output */
size_t fread(void* restrict ptr, size_t size, size_t nmemb, FILE* restrict stream);
size_t fwrite(const void* restrict ptr, size_t size, size_t nmemb, FILE* restrict stream);

/* file positioning */
int fgetpos(FILE* restrict stream, fpos_t* restrict pos);
int fseek(FILE* stream, long int offset, int whence);
int fsetpos(FILE* stream, const fpos_t* pos);
long int ftell(FILE* stream);
void rewind(FILE* stream);

/* error handling */
void clearerr(FILE* stream);
int feof(FILE* stream);
int ferror(FILE* stream);
void perror(const char* _Opt s);

#if defined(__linux__) || defined(__APPLE__)
/* POSIX */
FILE* _Owner _Opt popen(const char* command, const char* mode);
int pclose(FILE* _Owner stream);
int fileno(FILE* stream);
FILE* _Owner _Opt fdopen(int fd, const char* mode);
#elif defined(_WIN32)
FILE* _Owner _Opt _popen(const char* command, const char* mode);
int _pclose(FILE* _Owner stream);
int _fileno(FILE* stream);
FILE* _Owner _Opt _fdopen(int fd, const char* mode);
#endif

#else


#ifdef _WIN64
typedef struct _iobuf FILE;
typedef unsigned __int64 size_t;
#elif defined _WIN32
typedef struct _iobuf FILE;
typedef unsigned int     size_t;
#endif

#ifdef __linux__

typedef struct _IO_FILE FILE;
typedef __SIZE_TYPE__ size_t; // valid since C23

#endif

#ifdef __APPLE__

typedef struct __sFILE FILE;
typedef __SIZE_TYPE__ size_t;

#endif

int snprintf(_Out char* const _Buffer, size_t const _BufferCount, char const* const _Format, ...);

FILE* _Owner _Opt fopen(char const* _FileName, char const* _Mode);
int fclose(FILE* _Owner _Stream);

#if defined __linux__ || defined __APPLE__
FILE* _Owner _Opt popen(const char* _Command, const char* _Mode);
int pclose(FILE* _Owner _Stream);
#endif

size_t fread(
        _Out void* _Buffer,
        size_t _ElementSize,
        size_t _ElementCount,
        FILE* _Stream
);

#include_next <stdio.h>
#endif
