#include "fs.h"



#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include "error.h"
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#endif


#if defined _MSC_VER && !defined __POCC__
#include <crtdbg.h>
#include <debugapi.h>
#endif


#ifdef __POCC__
#define stat _stat
#endif

#ifdef _WIN32
#pragma comment (lib, "Rpcrt4.lib")

#else
#include <stdlib.h>
#include <unistd.h>
#include <uuid/uuid.h>
/*
caso nao tenha este arquivos apt-get install uuid-dev
*/
#endif

#include <assert.h>

#include <string.h>
#include <stdbool.h>
#include <errno.h>


bool path_is_absolute(const char* path)
{
#ifdef _WINDOWS_
    const char ch = tolower(path[0]);
    if (ch >= 'a' && ch <= 'z')
    {
        /*  c:/ or c:\ */
        if (path[1] == ':' && (path[2] == '\\' || path[2] == '/'))
            return true;
    }

    if (path[0] == '\\' && path[1] == '\\')
    {
        // //server
        return true;
    }
#else
    return path[0] == '/';
#endif

    return false;    
}

bool path_is_relative(const char* path)
{
    return !path_is_absolute(path);
}


#ifdef _WIN32

#ifdef __CAKE__
#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignore "-Wstyle"
#endif

struct TAGDIR
{
    void* handle;
    struct dirent dirent;
};

#ifdef __CAKE__
#pragma CAKE diagnostic pop
#endif

DIR* owner opendir(const char* name)
{
    assert(name != 0);
    WIN32_FIND_DATAA fdfile = {0};

    char path[MAX_PATH] = {0};
    strcat(path, name);
    strcat(path, "\\*.*");

    HANDLE handle = FindFirstFileA(path, &fdfile);

    if (handle != INVALID_HANDLE_VALUE)
    {
        DIR* owner p = calloc(1, sizeof * p);
        if (p)
        {
            p->handle = handle;
            return p;
        }
        else
        {
            /*calloc sets errno to ENOMEM if a memory allocation fails */
            FindClose(handle);
        }
    }
    else
    {
        errno = windows_error_to_posix(GetLastError());
    }

    return NULL;
}

int closedir(DIR* owner dirp)
{
    FindClose(dirp->handle);
    free(dirp);
    return 0;
}

struct dirent* readdir(DIR* dirp)
{
    WIN32_FIND_DATAA fdfile = {0};
    BOOL b = FindNextFileA(dirp->handle, &fdfile);
    if (b)
    {
        /*clear*/
        memset(&dirp->dirent, 0, sizeof(dirp->dirent));

        if (fdfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            dirp->dirent.d_type |= DT_DIR;
        }

        /*worst case trunks the string*/
        strncpy(dirp->dirent.d_name,
            fdfile.cFileName,
            sizeof(dirp->dirent.d_name) - 1);

        return &dirp->dirent;
    }
    else
    {
        errno = windows_error_to_posix(GetLastError());
    }
    return NULL;
}

/*
* realpath
   If there is no error, realpath() returns a pointer to the
       resolved_path.

       Otherwise, it returns NULL, the contents of the array
       resolved_path are undefined, and errno is set to indicate the
       error.
*/
char* realpath(const char* restrict path, char* restrict resolved_path)
{
    /*
    * _fullpath
    * Each of these functions returns a pointer to a buffer
      containing the absolute path name (absPath). If there's an
      error (for example, if the value passed in relPath includes a drive
      letter that isn't valid or can't be found, or if the length of the
      created absolute path name (absPath) is greater than maxLength), the function returns NULL.
    */
    char* p = _fullpath(resolved_path, path, MAX_PATH);
    char* p2 = resolved_path;
    while (*p2)
    {
        if (*p2 == '\\')
            *p2 = '/';
        p2++;
    }
    return p;
}

#endif //_WINDOWS_

int copy_file(const char* pathfrom, const char* pathto)
{

    FILE* owner fd_from = fopen(pathfrom, "rb");
    if (fd_from == NULL)
        return -1;

    FILE* owner fd_to = fopen(pathto, "wb");
    if (fd_to == NULL)
    {
        fclose(fd_from);
        return -1;
    }

    char buf[4096] = {0};
    size_t nread;
    while (nread = fread(buf, sizeof(char), sizeof buf, fd_from), nread > 0) //lint !e668  (warning -- possibly passing null pointer to function 'fread(void *, size_t, size_t, FILE *)', arg. no. 4)
    {
        char* out_ptr = buf;
        size_t nwritten;

        do
        {
            nwritten = fwrite(out_ptr, sizeof(char), nread, fd_to);//lint !e668

            nread -= nwritten;
            out_ptr += nwritten;
        } while (nread > 0);
    }

    if (nread == 0)
    {
        fclose(fd_to);
        fclose(fd_from);

        /* Success! */
        return 0;
    }

    fclose(fd_to);
    fclose(fd_from);

    return -1;
}

int copy_folder(const char* from, const char* to)
{
#if !defined __EMSCRIPTEN__
    int errcode = mkdir(to, 0700);
    if (errcode != 0)
    {
        return errcode;
    }

    DIR* owner dir = opendir(from);

    if (dir == NULL)
    {
        return errno;
    }

    struct dirent* dp;
    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        {
            /* skip self and parent */
            continue;
        }

        char fromlocal[MAX_PATH] = {0};
        snprintf(fromlocal, MAX_PATH, "%s/%s", from, dp->d_name);

        char tolocal[MAX_PATH] = {0};
        snprintf(tolocal, MAX_PATH, "%s/%s", to, dp->d_name);

        if (dp->d_type & DT_DIR)
        {
            errcode = copy_folder(fromlocal, tolocal);
        }
        else
        {
            errcode = copy_file(fromlocal, tolocal);
        }

        if (errcode != 0)
            break;
    }

    closedir(dir);
    return errcode;
#else
    return -1;
#endif
}

#ifdef _WIN32
int get_self_path(char* buffer, int maxsize)
{
    DWORD r = GetModuleFileNameA(NULL, buffer, maxsize);
    return r;
}

#else

int get_self_path(char* buffer, int maxsize)
{

    memset(buffer, 0, maxsize); // readlink does not null terminate!
    if (readlink("/proc/self/exe", buffer, maxsize) == -1)
    {
        //perror("readlink");
        return 1;
    }
    else
    {
        //printf("%s\n", dest);
    }
    return 0;

}

#endif




#if !defined __EMSCRIPTEN__
char* dirname(char* path)
{
    int last = -1;
    for (int i = 0; path[i]; i++)
    {
        if (path[i] == '\\' || path[i] == '/')
            last = i;
    }

    if (last != -1)
    {
        path[last] = 0;
    }
    else
    {
        path[0] = 0;
    }
    return path;
}
#endif

#ifndef MOCKFILES

char* owner read_file(const char* const path)
{
    char* owner data = NULL;
    FILE* owner file = NULL;
    struct stat info = {0};

    if (stat(path, &info) != 0)
        return NULL;

    const int mem_size_bytes = sizeof(char) * info.st_size + 3 /*BOM*/ + 1 /* \0 */;

    data = malloc(mem_size_bytes);
    if (data == NULL)
        return NULL;

    file = fopen(path, "r");
    if (file == NULL)
    {
        free(data);
        return NULL;
    }

    /* first we read 3 bytes */
    size_t bytes_read = fread(data, 1, 3, file);

    if (bytes_read < 3)
    {
        /* we have less than 3 bytes - no BOM */

        data[bytes_read] = '\0';
        if (feof(file))
        {
            fclose(file);
            return data;
        }

        free(data);
        fclose(file);

        return NULL;
    }

    /* check byte order mark (BOM) */
    if ((unsigned char) data[0] == (unsigned char) 0xEF &&
        (unsigned char) data[1] == (unsigned char) 0xBB &&
        (unsigned char) data[2] == (unsigned char) 0xBF)
    {
        /* in this case we skip this BOM */
        size_t bytes_read_part2 = fread(&data[0], 1, info.st_size - 3, file);
        data[bytes_read_part2] = 0;

        fclose(file);
        return data;
    }

    size_t bytes_read_part2 = fread(&data[3], 1, info.st_size - 3, file);
    data[bytes_read_part2 + 3] = 0;

    fclose(file);
    return data;
}

#else

static const char* file_assert_h =
"\n"
"#ifdef NDEBUG\n"
"#define assert(...) ((void)0)"
"#else"
"#define assert(...) assert(__VA_ARGS__)\n"
"#endif\n"
"\n"
"";


static const char* file_stdio_h =
"#pragma once\n"
"#define _IOFBF 0x0000\n"
"#define _IOLBF 0x0040\n"
"#define _IONBF 0x0004\n"
"\n"
"#define BUFSIZ  512\n"
"\n"
"#define EOF    (-1)\n"
"\n"
"#define FILENAME_MAX    260\n"
"#define FOPEN_MAX       20\n"
"\n"
"#define L_tmpnam   260 // _MAX_PATH\n"
"\n"
"/* Seek method constants */\n"
"\n"
"#define SEEK_CUR    1\n"
"#define SEEK_END    2\n"
"#define SEEK_SET    0\n"
"\n"
"\n"
"#define TMP_MAX         2147483647\n"
"\n"
"\n"
"#define stdin  ((FILE*)0)\n"
"#define stdout ((FILE*)1)\n"
"#define stderr ((FILE*)2)\n"
"\n"
"typedef long long fpos_t;\n"
"typedef int FILE;\n"
"\n"
"extern FILE* stdin;\n"
"extern FILE* stdout;\n"
"extern FILE* stderr;\n"
"\n"
"typedef int size_t;\n"
"typedef void* va_list;\n"
"int remove(const char* filename);\n"
"int rename(const char* old, const char* news);\n"
"FILE* tmpfile(void);\n"
"char* tmpnam(char* s);\n"
"#if defined(__STDC_OWNERSHIP__) && defined(__OWNERSHIP_H__)\n"
"int fclose(FILE* _Owner stream);\n"
"#else\n"
"int fclose(FILE* stream);\n"
"#endif\n"
"int fflush(FILE* stream);\n"
"#if defined(__STDC_OWNERSHIP__) && defined(__OWNERSHIP_H__)\n"
"FILE* _Owner fopen(const char* restrict filename, const char* restrict mode);\n"
"FILE* _Owner freopen(const char* restrict filename, const char* restrict mode, FILE* restrict stream);\n"
"#else\n"
"FILE* fopen(const char* restrict filename, const char* restrict mode);\n"
"FILE* freopen(const char* restrict filename, const char* restrict mode, FILE* restrict stream);\n"
"#endif\n"
"void setbuf(FILE* restrict stream, char* restrict buf);\n"
"int setvbuf(FILE* restrict stream, char* restrict buf, int mode, size_t size);\n"
"int fprintf(FILE* restrict stream, const char* restrict format, ...);\n"
"int fscanf(FILE* restrict stream, const char* restrict format, ...);\n"
"int printf(const char* restrict format, ...);\n"
"int scanf(const char* restrict format, ...);\n"
"int snprintf(char* restrict s, size_t n, const char* restrict format, ...);\n"
"int sprintf(char* restrict s, const char* restrict format, ...);\n"
"int sscanf(const char* restrict s, const char* restrict format, ...);\n"
"int vfprintf(FILE* restrict stream, const char* restrict format, va_list arg);\n"
"int vfscanf(FILE* restrict stream, const char* restrict format, va_list arg);\n"
"int vprintf(const char* restrict format, va_list arg);\n"
"int vscanf(const char* restrict format, va_list arg);\n"
"int puts(const char* str);\n"
"int fputs(const char* restrict s, FILE* restrict stream);\n"
"int getc(FILE* stream);\n"
"int getchar(void);\n"
"int putc(int c, FILE* stream);\n"
"int putchar(int c);\n"
"int puts(const char* s);\n"
"int ungetc(int c, FILE* stream);\n"
"int fgetc(FILE* stream);\n"
"size_t fread(void* restrict ptr, size_t size, size_t nmemb, FILE* restrict stream);\n"
"size_t fwrite(const void* restrict ptr, size_t size, size_t nmemb, FILE* restrict stream);\n"
"int fgetpos(FILE* restrict stream, fpos_t* restrict pos);\n"
"int fseek(FILE* stream, long int offset, int whence);\n"
"int fsetpos(FILE* stream, const fpos_t* pos);\n"
"long int ftell(FILE* stream);\n"
"void rewind(FILE* stream);\n"
"void clearerr(FILE* stream);\n"
"int feof(FILE* stream);\n"
"int ferror(FILE* stream);\n"
"void perror(const char* s);\n"
"\n"
"\n"
"\n"
"#ifndef NULL\n"
"#define NULL ((void*)0)\n"
"#endif\n"
"";

static const char* file_errno_h =
"#pragma once\n"
"\n"
"int* _errno(void);\n"
"#define errno (*_errno())\n"
"\n"
"\n"
"#define EPERM           1\n"
"#define ENOENT          2\n"
"#define ESRCH           3\n"
"#define EINTR           4\n"
"#define EIO             5\n"
"#define ENXIO           6\n"
"#define E2BIG           7\n"
"#define ENOEXEC         8\n"
"#define EBADF           9\n"
"#define ECHILD          10\n"
"#define EAGAIN          11\n"
"#define ENOMEM          12\n"
"#define EACCES          13\n"
"#define EFAULT          14\n"
"#define EBUSY           16\n"
"#define EEXIST          17\n"
"#define EXDEV           18\n"
"#define ENODEV          19\n"
"#define ENOTDIR         20\n"
"#define EISDIR          21\n"
"#define ENFILE          23\n"
"#define EMFILE          24\n"
"#define ENOTTY          25\n"
"#define EFBIG           27\n"
"#define ENOSPC          28\n"
"#define ESPIPE          29\n"
"#define EROFS           30\n"
"#define EMLINK          31\n"
"#define EPIPE           32\n"
"#define EDOM            33\n"
"#define EDEADLK         36\n"
"#define ENAMETOOLONG    38\n"
"#define ENOLCK          39\n"
"#define ENOSYS          40\n"
"#define ENOTEMPTY       41\n"
"\n"
"\n"
"// Support EDEADLOCK for compatibility with older Microsoft C versions\n"
"#define EDEADLOCK       EDEADLK\n"
"\n"
"#define EADDRINUSE      100\n"
"#define EADDRNOTAVAIL   101\n"
"#define EAFNOSUPPORT    102\n"
"#define EALREADY        103\n"
"#define EBADMSG         104\n"
"#define ECANCELED       105\n"
"#define ECONNABORTED    106\n"
"#define ECONNREFUSED    107\n"
"#define ECONNRESET      108\n"
"#define EDESTADDRREQ    109\n"
"#define EHOSTUNREACH    110\n"
"#define EIDRM           111\n"
"#define EINPROGRESS     112\n"
"#define EISCONN         113\n"
"#define ELOOP           114\n"
"#define EMSGSIZE        115\n"
"#define ENETDOWN        116\n"
"#define ENETRESET       117\n"
"#define ENETUNREACH     118\n"
"#define ENOBUFS         119\n"
"#define ENODATA         120\n"
"#define ENOLINK         121\n"
"#define ENOMSG          122\n"
"#define ENOPROTOOPT     123\n"
"#define ENOSR           124\n"
"#define ENOSTR          125\n"
"#define ENOTCONN        126\n"
"#define ENOTRECOVERABLE 127\n"
"#define ENOTSOCK        128\n"
"#define ENOTSUP         129\n"
"#define EOPNOTSUPP      130\n"
"#define EOTHER          131\n"
"#define EOVERFLOW       132\n"
"#define EOWNERDEAD      133\n"
"#define EPROTO          134\n"
"#define EPROTONOSUPPORT 135\n"
"#define EPROTOTYPE      136\n"
"#define ETIME           137\n"
"#define ETIMEDOUT       138\n"
"#define ETXTBSY         139\n"
"#define EWOULDBLOCK     140\n"
"\n"
"";



static const char* file_string_h =
" \n"
"typedef int errno_t;\n"
"typedef unsigned long long size_t;\n"
"typedef unsigned long long rsize_t;\n"
"typedef unsigned short wchar_t;\n"
"void* memchr(void const* _Buf, int _Val, size_t _MaxCount);\n"
"int memcmp(void const* _Buf1, void const* _Buf2, size_t _Size);\n"
"void* memcpy(void* _Dst, void const* _Src, size_t _Size);\n"
"void* memmove(void* _Dst, void const* _Src, size_t _Size);\n"
"void* memset(void* _Dst, int _Val, size_t _Size);\n"
"char* strchr(char const* _Str, int _Val);\n"
"char* strrchr(char const* _Str, int _Ch);\n"
"char* strstr(char const* _Str, char const* _SubStr);\n"
"wchar_t* wcschr(wchar_t const* _Str, wchar_t _Ch);\n"
"wchar_t* wcsrchr(wchar_t const* _Str, wchar_t _Ch);\n"
"wchar_t* wcsstr(wchar_t const* _Str, wchar_t const* _SubStr);\n"
"static __inline errno_t memcpy_s(void* const _Destination, rsize_t const _DestinationSize, void const* const _Source, rsize_t const _SourceSize);\n"
"static __inline errno_t memmove_s(void* const _Destination, rsize_t const _DestinationSize, void const* const _Source, rsize_t const _SourceSize);\n"
"int _memicmp(void const* _Buf1, void const* _Buf2, size_t _Size);\n"
"void* memccpy(void* _Dst, void const* _Src, int _Val, size_t _Size);\n"
"int memicmp(void const* _Buf1, void const* _Buf2, size_t _Size);\n"
"errno_t wcscat_s(wchar_t* _Destination, rsize_t _SizeInWords, wchar_t const* _Source);\n"
"errno_t wcscpy_s(wchar_t* _Destination, rsize_t _SizeInWords, wchar_t const* _Source);\n"
"errno_t wcsncat_s(wchar_t* _Destination, rsize_t _SizeInWords, wchar_t const* _Source, rsize_t _MaxCount);\n"
"errno_t wcsncpy_s(wchar_t* _Destination, rsize_t _SizeInWords, wchar_t const* _Source, rsize_t _MaxCount);\n"
"wchar_t* wcstok_s(wchar_t* _String, wchar_t const* _Delimiter, wchar_t** _Context);\n"
"wchar_t* _wcsdup(wchar_t const* _String);\n"
"wchar_t* wcscat(wchar_t* _Destination, wchar_t const* _Source); int wcscmp(wchar_t const* _String1, wchar_t const* _String2);\n"
"wchar_t* wcscpy(wchar_t* _Destination, wchar_t const* _Source); size_t wcscspn(wchar_t const* _String, wchar_t const* _Control);\n"
"size_t wcslen(wchar_t const* _String);\n"
"size_t wcsnlen(wchar_t const* _Source, size_t _MaxCount);\n"
"static __inline size_t wcsnlen_s(wchar_t const* _Source, size_t _MaxCount);\n"
"wchar_t* wcsncat(wchar_t* _Destination, wchar_t const* _Source, size_t _Count);\n"
"int wcsncmp(wchar_t const* _String1, wchar_t const* _String2, size_t _MaxCount);\n"
"wchar_t* wcsncpy(wchar_t* _Destination, wchar_t const* _Source, size_t _Count);\n"
"wchar_t* wcspbrk(wchar_t const* _String, wchar_t const* _Control);\n"
"size_t wcsspn(wchar_t const* _String, wchar_t const* _Control);\n"
"wchar_t* wcstok(wchar_t* _String, wchar_t const* _Delimiter, wchar_t** _Context);\n"
"size_t wcsxfrm(wchar_t* _Destination, wchar_t const* _Source, size_t _MaxCount);\n"
"int wcscoll(wchar_t const* _String1, wchar_t const* _String2);\n"
"wchar_t* wcsdup(wchar_t const* _String);\n"
"int wcsicmp(wchar_t const* _String1, wchar_t const* _String2);\n"
"int wcsnicmp(wchar_t const* _String1, wchar_t const* _String2, size_t _MaxCount);\n"
"wchar_t* wcsnset(wchar_t* _String, wchar_t _Value, size_t _MaxCount);\n"
"wchar_t* wcsrev(wchar_t* _String);\n"
"wchar_t* wcsset(wchar_t* _String, wchar_t _Value);\n"
"wchar_t* wcslwr(wchar_t* _String); wchar_t* wcsupr(wchar_t* _String);\n"
"int wcsicoll(wchar_t const* _String1, wchar_t const* _String2);\n"
"char* strtok_s(char* _String, char const* _Delimiter, char** _Context);\n"
"void* _memccpy(void* _Dst, void const* _Src, int _Val, size_t _MaxCount);\n"
"char* strcat(char* _Destination, char const* _Source);\n"
"int strcmp(char const* _Str1, char const* _Str2);\n"
"int strcoll(char const* _String1, char const* _String2);\n"
"char* strerror(int _ErrorMessage);\n"
"size_t strlen(char const* _Str);\n"
"char* strncat(char* _Destination, char const* _Source, size_t _Count);\n"
"int strncmp(char const* _Str1, char const* _Str2, size_t _MaxCount);\n"
"char* strncpy(char* _Destination, char const* _Source, size_t _Count);\n"
"size_t strnlen(char const* _String, size_t _MaxCount);\n"
"static __inline size_t strnlen_s(char const* _String, size_t _MaxCount);\n"
"char* strpbrk(char const* _Str, char const* _Control);\n"
"size_t strspn(char const* _Str, char const* _Control);\n"
"char* strtok(char* _String, char const* _Delimiter);\n"
"#if defined(__STDC_OWNERSHIP__) && defined(__OWNERSHIP_H__)\n"
"char* _Owner strdup(char const* _String);\n"
"#else\n"
"char* strdup(char const* _String);\n"
"#endif\n"
"int strcmpi(char const* _String1, char const* _String2);\n"
"int stricmp(char const* _String1, char const* _String2);\n"
"char* strlwr(char* _String);\n"
"int strnicmp(char const* _String1, char const* _String2, size_t _MaxCount);\n"
"char* strnset(char* _String, int _Value, size_t _MaxCount);\n"
"char* strrev(char* _String);\n"
"char* strset(char* _String, int _Value); char* strupr(char* _String);";


static const char* file_math_h
=
"#pragma once\n"
"\n"
"double acos(double __x);\n"
"double asin(double __x);\n"
"double atan(double __x);\n"
"double atan2(double __y, double __x);\n"
"double cos(double __x);\n"
"double sin(double __x);\n"
"double tan(double __x);\n"
"double cosh(double __x);\n"
"double sinh(double __x);\n"
"double tanh(double __x);\n"
"double acosh(double __x);\n"
"double asinh(double __x);\n"
"double atanh(double __x);\n"
"double exp(double __x);\n"
"double frexp(double __x, int* __exponent);\n"
"double ldexp(double __x, int __exponent);\n"
"double log(double __x);\n"
"double log10(double __x);\n"
"double modf(double __x, double* __iptr);\n"
"double expm1(double __x);\n"
"double log1p(double __x);\n"
"double logb(double __x);\n"
"double exp2(double __x);\n"
"double log2(double __x);\n"
"double pow(double __x, double __y);\n"
"double sqrt(double __x);\n"
"double hypot(double __x, double __y);\n"
"double cbrt(double __x);\n"
"double ceil(double __x);\n"
"double fabs(double __x);\n"
"double floor(double __x);\n"
"double fmod(double __x, double __y);\n"
"int isinf(double __value);\n"
"int finite(double __value);\n"
"double drem(double __x, double __y);\n"
"double significand(double __x);\n"
"double copysign(double __x, double __y);\n"
"double nan(const char* __tagb);\n"
"int isnan(double __value);\n"
"double j0(double);\n"
"double j1(double);\n"
"double jn(int, double);\n"
"double y0(double);\n"
"double y1(double);\n"
"double yn(int, double);\n"
"double erf(double);\n"
"double erfc(double);\n"
"double lgamma(double);\n"
"double tgamma(double);\n"
"double gamma(double);\n"
"double lgamma_r(double, int* __signgamp);\n"
"double rint(double __x);\n"
"double nextafter(double __x, double __y);\n"
"double nexttoward(double __x, long double __y);\n"
"double remainder(double __x, double __y);\n"
"double scalbn(double __x, int __n);\n"
"int ilogb(double __x);\n"
"double scalbln(double __x, long int __n);\n"
"double nearbyint(double __x);\n"
"double round(double __x);\n"
"double trunc(double __x);\n"
"double remquo(double __x, double __y, int* __quo);\n"
"long int lrint(double __x);\n"
"long long int llround(double __x);\n"
"double fdim(double __x, double __y);\n"
"double fmax(double __x, double __y);\n"
"double fmin(double __x, double __y);\n"
"double fma(double __x, double __y, double __z);\n"
"double scalb(double __x, double __n);\n"
"float acosf(float __x);\n"
"float asinf(float __x);\n"
"float atanf(float __x);\n"
"float atan2f(float __y, float __x);\n"
"float cosf(float __x);\n"
"float sinf(float __x);\n"
"float tanf(float __x);\n"
"float coshf(float __x);\n"
"float sinhf(float __x);\n"
"float tanhf(float __x);\n"
"float acoshf(float __x);\n"
"float asinhf(float __x);\n"
"float atanhf(float __x);\n"
"float expf(float __x);\n"
"float frexpf(float __x, int* __exponent);\n"
"float ldexpf(float __x, int __exponent);\n"
"float logf(float __x);\n"
"float log10f(float __x); float __log10f(float __x);\n"
"float modff(float __x, float* __iptr);\n"
"float expm1f(float __x);\n"
"float log1pf(float __x);\n"
"float logbf(float __x);\n"
"float exp2f(float __x);\n"
"float log2f(float __x);\n"
"float powf(float __x, float __y);\n"
"float sqrtf(float __x);\n"
"float hypotf(float __x, float __y);\n"
"float cbrtf(float __x);\n"
"float ceilf(float __x);\n"
"float fabsf(float __x);\n"
"float floorf(float __x);\n"
"float fmodf(float __x, float __y); \n"
"int isinff(float __value);\n"
"int finitef(float __value);\n"
"float dremf(float __x, float __y);\n"
"float significandf(float __x); \n"
"float copysignf(float __x, float __y); \n"
"float nanf(const char* __tagb); \n"
"int isnanf(float __value);\n"
"float j0f(float); \n"
"float j1f(float); \n"
"float jnf(int, float); \n"
"float y0f(float); \n"
"float y1f(float); \n"
"float ynf(int, float); \n"
"float erff(float); \n"
"float erfcf(float);\n"
"float lgammaf(float);\n"
"float tgammaf(float);\n"
"float gammaf(float); \n"
"float lgammaf_r(float, int* __signgamp); \n"
"float rintf(float __x); \n"
"float nextafterf(float __x, float __y); \n"
"float nexttowardf(float __x, long double __y); \n"
"float remainderf(float __x, float __y); \n"
"float scalbnf(float __x, int __n); \n"
"int ilogbf(float __x); \n"
"float scalblnf(float __x, long int __n); \n"
"float nearbyintf(float __x); \n"
"float roundf(float __x); \n"
"float truncf(float __x); \n"
"float remquof(float __x, float __y, int* __quo); \n"
"long int lrintf(float __x); \n"
"long long int llroundf(float __x); \n"
"float fdimf(float __x, float __y);\n"
"float fmaxf(float __x, float __y); \n"
"float fminf(float __x, float __y);\n"
"float fmaf(float __x, float __y, float __z); \n"
"float scalbf(float __x, float __n); \n"
"long double acosl(long double __x); \n"
"long double asinl(long double __x); \n"
"long double atanl(long double __x); \n"
"long double atan2l(long double __y, long double __x); \n"
"long double cosl(long double __x); \n"
"long double sinl(long double __x); \n"
"long double tanl(long double __x); \n"
"long double coshl(long double __x);\n"
"long double sinhl(long double __x);\n"
"long double tanhl(long double __x);\n"
"long double acoshl(long double __x); \n"
"long double asinhl(long double __x); \n"
"long double atanhl(long double __x); \n"
"long double expl(long double __x); \n"
"long double frexpl(long double __x, int* __exponent); \n"
"long double ldexpl(long double __x, int __exponent); \n"
"long double logl(long double __x); \n"
"long double log10l(long double __x); \n"
"long double modfl(long double __x, long double* __iptr); \n"
"long double expm1l(long double __x); \n"
"long double log1pl(long double __x); \n"
"long double logbl(long double __x); \n"
"long double exp2l(long double __x); \n"
"long double log2l(long double __x); \n"
"long double powl(long double __x, long double __y); \n"
"long double sqrtl(long double __x); \n"
"long double hypotl(long double __x, long double __y); \n"
"long double cbrtl(long double __x); \n"
"long double ceill(long double __x); \n"
"long double fabsl(long double __x); \n"
"long double floorl(long double __x);\n"
"long double fmodl(long double __x, long double __y); \n"
"int isinfl(long double __value);\n"
"int finitel(long double __value);\n"
"long double dreml(long double __x, long double __y); \n"
"long double significandl(long double __x); \n"
"long double copysignl(long double __x, long double __y); \n"
"long double nanl(const char* __tagb); \n"
"int isnanl(long double __value);\n"
"long double j0l(long double); \n"
"long double j1l(long double); \n"
"long double jnl(int, long double);\n"
"long double y0l(long double); \n"
"long double y1l(long double); \n"
"long double ynl(int, long double);\n"
"long double erfl(long double); \n"
"long double erfcl(long double);\n"
"long double lgammal(long double); \n"
"long double tgammal(long double); \n"
"long double gammal(long double); \n"
"long double lgammal_r(long double, int* __signgamp); \n"
"long double rintl(long double __x); \n"
"long double nextafterl(long double __x, long double __y); \n"
"long double nexttowardl(long double __x, long double __y);\n"
"long double remainderl(long double __x, long double __y); \n"
"long double scalbnl(long double __x, int __n); \n"
"int ilogbl(long double __x); \n"
"long double scalblnl(long double __x, long int __n); \n"
"long double nearbyintl(long double __x);\n"
"long double roundl(long double __x);\n"
"long double truncl(long double __x);\n"
"long double remquol(long double __x, long double __y, int* __quo);\n"
"long int lrintl(long double __x);\n"
"long long int llroundl(long double __x);\n"
"long double fdiml(long double __x, long double __y);\n"
"long double fmaxl(long double __x, long double __y);\n"
"long double fminl(long double __x, long double __y);\n"
"long double fmal(long double __x, long double __y, long double __z);\n"
"long double scalbl(long double __x, long double __n);\n"
"";

static const char* file_stdlib_h =
"typedef long long fpos_t;\n"
"typedef unsigned size_t;\n"
"\n"
"#define NULL ((void*)0)\n"
"typedef int wchar_t;\n"
"[[nodiscard]] double atof(const char* nptr);\n"
"[[nodiscard]] int atoi(const char* nptr);\n"
"[[nodiscard]] long int atol(const char* nptr);\n"
"[[nodiscard]] long long int atoll(const char* nptr);\n"
"double strtod(const char* restrict nptr, char** restrict endptr);\n"
"float strtof(const char* restrict nptr, char** restrict endptr);\n"
"long double strtold(const char* restrict nptr, char** restrict endptr);\n"
"long int strtol(const char* restrict nptr, char** restrict endptr, int base);\n"
"long long int strtoll(const char* restrict nptr, char** restrict endptr, int base);\n"
"unsigned long int strtoul(const char* restrict nptr, char** restrict endptr, int base);\n"
"unsigned long long int strtoull(const char* restrict nptr, char** restrict endptr, int base);\n"
"int rand(void);\n"
"void srand(unsigned int seed);\n"
"void* aligned_alloc(size_t alignment, size_t size);\n"
"#if defined(__STDC_OWNERSHIP__) && defined(__OWNERSHIP_H__)\n"
"[[nodiscard]] void* _Owner calloc(size_t nmemb, size_t size);\n"
"void free(void* _Owner ptr);\n"
"[[nodiscard]] void* _Owner malloc(size_t size);\n"
"[[nodiscard]] void* _Owner realloc(void* ptr, size_t size);\n"
"#else\n"
"[[nodiscard]] void* calloc(size_t nmemb, size_t size);\n"
"void free(void* ptr);\n"
"[[nodiscard]] void* malloc(size_t size);\n"
"[[nodiscard]] void* realloc(void* ptr, size_t size);\n"
"#endif\n"
"[[noreturn]] void abort(void);\n"
"int atexit(void (*func)(void));\n"
"int at_quick_exit(void (*func)(void));\n"
"[[noreturn]] void exit(int status);\n"
"[[noreturn]] void _Exit(int status);\n"
"char* getenv(const char* name);\n"
"[[noreturn]] void quick_exit(int status);\n"
"int system(const char* string);";

static const char* file_stddef_h =
"\n"
"typedef long int ptrdiff_t;\n"
"typedef long unsigned int size_t;\n"
"typedef int wchar_t;\n"
"typedef struct {\n"
"  long long __max_align_ll;\n"
"  long double __max_align_ld;\n"
"} max_align_t;\n"
"\n"
"typedef typeof(nullptr) nullptr_t;\n"
"\n";

const char* file_ownership_h = 
 "#ifndef __OWNERSHIP_H__\n"
 "#define __OWNERSHIP_H__\n"
 "\n"
 "#ifdef __STDC_OWNERSHIP__\n"
 "#define out _Out\n"
 "#define opt _Opt\n"
 "#define owner _Owner\n"
 "#define obj_owner  _Obj_owner\n"
 "#define view _View\n"
 "#define unchecked \"unchecked\"\n"
 "\n"
 "#else\n"
 "#define out \n"
 "#define opt \n"
 "#define owner\n"
 "#define obj_owner\n"
 "#define view\n"
 "#define static_debug(x)\n"
 "#define static_set(x, s)\n"
 "#define unchecked\n"
 "#endif\n"
 "\n"
 "#endif\n"
 "\n"
 "";



char* owner read_file(const char* path)
{
    if (strcmp(path, "c:/stdio.h") == 0)
        return strdup(file_stdio_h);
    else if (strcmp(path, "c:/stdlib.h") == 0)
        return strdup(file_stdlib_h);
    else if (strcmp(path, "c:/stddef.h") == 0)
        return strdup(file_stddef_h);
    else if (strcmp(path, "c:/math.h") == 0)
        return strdup(file_math_h);
    else if (strcmp(path, "c:/errno.h") == 0)
        return strdup(file_errno_h);
    else if (strcmp(path, "c:/string.h") == 0)
        return strdup(file_string_h);
    else if (strcmp(path, "c:/assert.h") == 0)
        return strdup(file_assert_h);
    else if (strcmp(path, "c:/ownership.h") == 0)
        return strdup(file_ownership_h);
    printf("read %s\n", path);
    return NULL;
}
#endif


