
#ifndef __OWNERSHIP_H__
#define __OWNERSHIP_H__

#ifdef __CAKE__


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

/*
  ownership is suported
*/
void* _Owner _Opt _Clear calloc(size_t nmemb, size_t size);
void free(void* _Owner _Opt ptr);
void* _Owner _Opt _Uninitialized malloc(size_t size);
void* _Owner _Opt realloc(void* _Opt ptr, size_t size);
char* _Owner _Opt strdup(const char* src);
char* _Opt strstr(const char* str, const char* substr);

inline char* _Opt strrchr(char const *  _String, int _Ch);

#ifdef _WIN32
__inline int __cdecl snprintf(_Ctor char* const _Buffer, size_t  const _BufferCount, char const* const _Format, ...);

char* _Opt _fullpath(
   char* _Opt absPath,
   const char* relPath,
   size_t maxLength
);

#else

int snprintf(
        _Ctor char*       const _Buffer,
        size_t      const _BufferCount,
        char const* const _Format,
        ...);
#endif

long strtol(
    char const* _String,
    char**     _Opt _EndPtr,
    int         _Radix
    );


FILE* _Owner _Opt fopen(char const* _FileName, char const* _Mode);
int fclose(FILE* _Owner _Stream);

#if defined __linux__ || defined __APPLE__
FILE* _Owner _Opt popen(const char* _Command, const char* _Mode);
int pclose(FILE* _Owner _Stream);
#endif

size_t fread(
        _Ctor void*  _Buffer,
        size_t _ElementSize,
        size_t _ElementCount,
        FILE*  _Stream
        );

long long strtoll(
    char const* _String,
    char** _Opt _EndPtr,
    int         _Radix
    );

double strtod(
    char const* _String,
    char**      _Opt _EndPtr
    );

long double strtold(char const* _String,char** _Opt _EndPtr);


unsigned long long strtoull(
    char const* _String,
    char**      _Opt _EndPtr,
    int         _Radix
    );

float strtof(char const* _String, char** _Opt _EndPtr);

//typedef unsigned long long time_t;
//static time_t time(time_t* const _Opt _Time);

#else
/*
  ownership not suported
*/

#define _Ctor
#define _Opt
#define _Owner
#define _Dtor
#define _View
#define _Clear
#define _Uninitialized
#define static_debug(x)
#define override_state(x, s)
#define _Assert(x) ((void)0)
#endif

#endif

#ifdef _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
#endif

