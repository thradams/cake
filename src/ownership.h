
#pragma once



#ifdef __CAKE__

#define owner _Owner
#define obj_owner  _Obj_owner
#define view _View
#define static_debug(x)
#define static_set(x, s)
#define unchecked

void* owner calloc(int nmemb, int size);
void free(void* owner ptr);
void* owner malloc(int size);
void* owner realloc(void*  ptr, int size);
char * owner strdup( const char *src );

typedef struct _iobuf FILE;

FILE* owner fopen(char const* _FileName, char const* _Mode);
int fclose(FILE* owner _Stream);

char * owner strdup( const char *str1 );
#define unchecked "unchecked"
#else
#define owner
#define obj_owner
#define view
#define static_debug(x)
#define static_set(x, s)
#define unchecked
#endif

