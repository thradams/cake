

#ifndef __OWNERSHIP_H__
#define __OWNERSHIP_H__

#ifdef __STDC_OWNERSHIP__
 /*
   ownership is suported
 */
#define out _Out
#define opt _Opt
#define owner _Owner
#define obj_owner  _Obj_owner
#define view _View
#define unchecked "unchecked"


void* owner opt calloc(int nmemb, int size);
void free(void* owner opt ptr);
void* owner opt malloc(int size);
void* owner opt realloc(void*  ptr, int size);
char * owner opt strdup( const char *src );

typedef struct _iobuf FILE;
FILE* owner fopen(char const* _FileName, char const* _Mode);
int fclose(FILE* owner _Stream);

#else
#define out
#define opt
#define owner
#define obj_owner
#define view
#define static_debug(x)
#define static_set(x, s)
#define unchecked
#endif

#endif

