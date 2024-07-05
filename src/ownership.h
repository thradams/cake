
#ifndef __OWNERSHIP_H__
#define __OWNERSHIP_H__

#ifdef __STDC_OWNERSHIP__

typedef typeof(sizeof(0)) size_t; // valid since C23

/*
  ownership is suported
*/
void* _Owner _Opt calloc(int nmemb, size_t size);
void free(void* _Owner _Opt ptr);
void* _Owner _Opt malloc(size_t size);
void* _Owner _Opt realloc(void* _Opt ptr, size_t size);
char* _Owner _Opt strdup(const char* src);

int snprintf(_Out char* const _Buffer,
             size_t const _BufferCount,
             char const* const _Format,
             ...);

typedef struct _iobuf FILE;
FILE* _Owner fopen(char const* _FileName, char const* _Mode);
int fclose(FILE* _Owner _Stream);

size_t fread(
        _Out void*  _Buffer,
        size_t _ElementSize,
        size_t _ElementCount,
        FILE*  _Stream
        );

long long strtoll(
    char const* _String,
    char** _Opt _EndPtr,
    int         _Radix
    );

#else
/*
  ownership not suported
*/

#define _Out
#define _Opt
#define _Owner
#define _Obj_owner
#define _View
#define static_debug(x)
#define static_set(x, s)
#endif

#endif

