
#ifndef __OWNERSHIP_H__
#define __OWNERSHIP_H__

#ifdef __STDC_OWNERSHIP__
/*
  ownership is suported
*/
void* _Owner _Opt calloc(int nmemb, int size);
void free(void* _Owner _Opt ptr);
void* _Owner _Opt malloc(int size);
void* _Owner _Opt realloc(void* ptr, int size);
char* _Owner _Opt strdup(const char* src);

typedef struct _iobuf FILE;
FILE* _Owner fopen(char const* _FileName, char const* _Mode);
int fclose(FILE* _Owner _Stream);

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

